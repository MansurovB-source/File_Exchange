//
// Created by behruz on 14.06.2021.
//

#include "udp_server.h"
#include "context.h"
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "tcp_server.h"

#define PORT 8080
#define BUFFER_SIZE 2048

void *start_server_udp(void *data) {
    struct context *ctx = data;

    //TODO get data from data {list and exit flag ?}

    int socket_fd;
    if ((socket_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Can't create socket");
        exit(-1);
    }

    struct sockaddr_in server_address;
    struct sockaddr_in client_address;

    memset(&server_address, 0, sizeof(struct sockaddr_in));
    memset(&client_address, 0, sizeof(struct sockaddr_in));

    int broadcast = 1;

    setsockopt(socket_fd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast));


    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = INADDR_ANY;

    if (bind(socket_fd, (const struct sockaddr *) &server_address, sizeof(server_address)) < 0) {
        perror("Binding is failed");
        exit(-1);
    }

    printf("Successfully started server on PORT: %d\n", PORT);

    char buffer[BUFFER_SIZE];
    size_t client_address_size = sizeof(client_address);

    while (!ctx->exit) {
        if ((recvfrom(socket_fd, (char *) buffer, BUFFER_SIZE, MSG_WAITALL, (struct sockaddr *) &client_address,
                      (socklen_t *) &client_address_size)) > BUFFER_SIZE) {
            printf("TOO LONG MESSAGE\n");
        }
    }

    printf("REQUEST: %s\n", buffer);

    struct file_triplet *triplet = find_triplet(ctx->l, buffer);

    struct udp_server_answer answer = {0};

    if (triplet) {
        answer.success = 1;
        answer.port = 7898;
        answer.triplet = *triplet;

        //TODO tcp client
        pthread_t tcp_client;
        struct tcp_server_data *server_data = malloc(sizeof(struct tcp_server_data));
        server_data->udp_server_ans = answer;
        server_data->ctx = NULL;
        pthread_create(&tcp_client, NULL, start_server_tcp, server_data);
    }

    sendto(socket_fd, &answer, sizeof(struct udp_server_answer),
           MSG_CONFIRM, (const struct sockaddr *) &client_address,
           client_address_size);

    close(socket_fd);
    return NULL;
}
