//
// Created by behruz on 17.06.2021.
//

#ifndef FILE_EXCHANGE_TCP_SERVER_H
#define FILE_EXCHANGE_TCP_SERVER_H

#include <netinet/in.h>
#include "context.h"
#include "udp_server.h"

struct tcp_server_data {
    int32_t socket_fd;
    uint16_t port;
    struct sockaddr_in server_address;
    struct sockaddr_in client_address;
    struct file_triplet *triplet;
};

struct tcp_server_request {
    char command[4];
    size_t arg;
};

void *start_server_tcp(void *data);

void init_tcp_server(struct tcp_server_data *server_data);

#endif //FILE_EXCHANGE_TCP_SERVER_H
