//
// Created by behruz on 14.06.2021.
//

#include "udp_server.h"
#include "context.h"
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include "tcp_server.h"

#define PORT 8080
#define BUFFER_SIZE 2048

void *start_server_udp(void *data) {
    struct context *ctx = data;

    //TODO get data from data {list and exit flag ?}

    int socket_fd;
    if ((socket_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("[UDP SERVER]: {ERROR} Socket creation failed");
        exit(-1);
    }

    struct sockaddr_in server_address;
    struct sockaddr_in client_address;

    memset(&server_address, 0, sizeof(struct sockaddr_in));
    memset(&client_address, 0, sizeof(struct sockaddr_in));

    int broadcast = 1;
    setsockopt(socket_fd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast));

    uint16_t port = PORT;

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = INADDR_ANY;

    while (bind(socket_fd, (const struct sockaddr *) &server_address, sizeof(server_address)) < 0) {
        char fail[128] = {0};
        sprintf(fail, "[UDP SERVER]: {ERROR} Binding on port %d failed\n", port++);
        put_action(ctx->events, fail);
        server_address.sin_port = htons(port);
    }

    char success[128] = {0};
    sprintf(success, "[UDP SERVER]: Successfully started server on PORT: %d!\n", port);
    put_action(ctx->events, success);

    char buffer[BUFFER_SIZE];
    size_t client_address_size = sizeof(client_address);

    while (!ctx->exit) {
        memset(buffer, 0, BUFFER_SIZE);
        if ((recvfrom(socket_fd, (char *) buffer, BUFFER_SIZE, MSG_WAITALL, (struct sockaddr *) &client_address,
                      (socklen_t *) &client_address_size)) > BUFFER_SIZE) {
            put_action(ctx->events, "[UDP SERVER]: {ERROR} TOO LONG MESSAGE\n");
        }

        //printf("[UDP SERVER]: REQUEST: %s\n", buffer);

        struct file_triplet *triplet = find_triplet(ctx->l, buffer);

        if (triplet) {
            struct udp_server_answer answer = {0};
            //TODO tcp client
            pthread_t *tcp_client = (pthread_t *) malloc(sizeof(pthread_t));
            struct tcp_server_data *server_data = malloc(sizeof(struct tcp_server_data));
            server_data->triplet = triplet;
            server_data->ctx = ctx;
            init_server_tcp(server_data);

            answer.success = 1;
            answer.port = server_data->port;
            answer.triplet.filesize = triplet->filesize;
            //TODO hash size ?
            strncpy(answer.triplet.hash, triplet->hash, 32);
            strcpy(answer.triplet.filename, triplet->filename);

            pthread_create(tcp_client, NULL, start_server_tcp, server_data);

            sendto(socket_fd, &answer, sizeof(struct udp_server_answer),
                   MSG_CONFIRM, (const struct sockaddr *) &client_address,
                   client_address_size);

        }
    }
    return NULL;
}
