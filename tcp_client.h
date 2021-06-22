//
// Created by behruz on 17.06.2021.
//

#ifndef FILE_EXCHANGE_TCP_CLIENT_H
#define FILE_EXCHANGE_TCP_CLIENT_H

#include <stdint.h>
#include <netinet/in.h>
#include "read_file.h"
#include "context.h"

struct tcp_server_answer {
    uint16_t len;
    int8_t payload[4096];
};

struct tcp_client_data {
    uint16_t port;
    struct file_triplet_dto triplet;
    in_addr_t server_addr;
    struct context *ctx;
};

void *start_tcp_client(void *thread_data);

#endif //FILE_EXCHANGE_TCP_CLIENT_H
