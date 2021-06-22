//
// Created by behruz on 17.06.2021.
//

#ifndef FILE_EXCHANGE_TCP_SERVER_H
#define FILE_EXCHANGE_TCP_SERVER_H

#include "../utils/context.h"
#include "udp_server.h"
#include <netinet/in.h>
#include <sys/socket.h>

struct tcp_server_request {
    char cmd[3];
    size_t arg;
};

struct tcp_server_data {
    int32_t sockfd;
    uint16_t port;
    struct sockaddr_in servaddr;
    struct sockaddr_in client;
    struct file_triplet *triplet;
    struct context *ctx;
};

void init_tcp_server(struct tcp_server_data *server_data);

void *start_tcp_server(void *thread_data);

#endif //FILE_EXCHANGE_TCP_SERVER_H
