//
// Created by behruz on 17.06.2021.
//

#ifndef FILE_EXCHANGE_TCP_SERVER_H
#define FILE_EXCHANGE_TCP_SERVER_H

#include "context.h"
#include "udp_server.h"

struct tcp_server_data {
    struct context *ctx;
    struct udp_server_answer udp_server_ans;
};

struct tcp_server_request {
    char command[4];
    size_t arg;
};

void *start_server_tcp(void *data);

#endif //FILE_EXCHANGE_TCP_SERVER_H
