//
// Created by behruz on 14.06.2021.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#include "udp_server.h"
#include "../utils/context.h"
#include "tcp_server.h"

#define PORT 8080
#define BUF_SIZE 1024

struct file_triplet *find_triplet(struct list *triplet_list, char *str) {
    struct list *element = triplet_list;
    while (element->value != NULL) {
        char triplet_str[1024] = {0};
        struct file_triplet *triplet = ((struct file_triplet *) element->value);
        strcat(triplet_str, triplet->filename);
        strcat(triplet_str, ":");
        char snum[1024];
        sprintf(snum, "%ld", triplet->filesize);
        strcat(triplet_str, snum);
        strcat(triplet_str, ":");
        strncat(triplet_str, triplet->hash, SHA256_DIGEST_LENGTH);
        if (!strcmp(str, triplet_str)) {
            return triplet;
        }
        element = element->next;
    }
    return NULL;
}

void *start_udp_server(void *thread_data) {
    struct context *ctx = thread_data;
    int socket_fd;
    char buffer[BUF_SIZE] = {0};
    struct sockaddr_in server_address, client_address;
    if ((socket_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    int broadcast = 1;

    uint16_t port = PORT;

    setsockopt(socket_fd, SOL_SOCKET, SO_BROADCAST,
               &broadcast, sizeof broadcast);

    memset(&server_address, 0, sizeof(server_address));
    memset(&client_address, 0, sizeof(client_address));

    // Filling server information
    server_address.sin_family = AF_INET; // IPv4
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(port);

    // Bind the socket with the server address
    while (bind(socket_fd, (const struct sockaddr *) &server_address,
                sizeof(server_address)) < 0) {
        char bind_failed[64] = {0};
        sprintf(bind_failed, "[UDP-server] {ERROR} Binding on port %d is failed", port++);
        put_action(ctx->events_module, bind_failed);
        server_address.sin_port = htons(port);
    }

    char bind_successful[64] = {0};
    sprintf(bind_successful, "[UDP-server] Successfully started server on port %d!", port);
    put_action(ctx->events_module, bind_successful);

    uint32_t len, n;

    len = sizeof(client_address);

    while (!ctx->exit) {
        memset(buffer, 0, BUF_SIZE);
        n = recvfrom(socket_fd, (char *) buffer, BUF_SIZE,
                     MSG_WAITALL, (struct sockaddr *) &client_address,
                     &len);

        if (n > BUF_SIZE) {
            put_action(ctx->events_module, "[UDP-server] {ERROR} Too long message");
        }

        put_action(ctx->events_module, "[UDP-server] client");

        struct file_triplet *pTriplet = find_triplet(ctx->triplet_list, buffer);

        if (pTriplet) {
            struct udp_server_answer answer = {0};

            pthread_t *tcp_server = (pthread_t *) malloc(sizeof(pthread_t));
            struct tcp_server_data *server_data = malloc(sizeof(struct tcp_server_data));
            server_data->triplet = pTriplet;
            server_data->ctx = ctx;
            init_tcp_server(server_data);

            answer.success = 1;
            answer.port = server_data->port;
            answer.triplet.filesize = pTriplet->filesize;
            strncpy(answer.triplet.hash, pTriplet->hash, 32);
            strcpy(answer.triplet.filename, pTriplet->filename);

            pthread_create(tcp_server, NULL, start_tcp_server, server_data);

            sendto(socket_fd, &answer, sizeof(struct udp_server_answer),
                   MSG_CONFIRM, (const struct sockaddr *) &client_address,
                   len);
        }
    }

    return NULL;
}
