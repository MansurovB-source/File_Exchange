//
// Created by behruz on 17.06.2021.
//

#ifndef FILE_EXCHANGE_TCP_CLIENT_H
#define FILE_EXCHANGE_TCP_CLIENT_H

#include "file_triplet.h"
struct tcp_server_answer {
    uint16_t len;
    int8_t payload[4096];
};

struct tcp_client_data {
    uint16_t port;
    struct file_triplet_dto triplet_dto;
    in_addr_t server_address;
};

void *start_client_tcp(void *data);

#endif //FILE_EXCHANGE_TCP_CLIENT_H
