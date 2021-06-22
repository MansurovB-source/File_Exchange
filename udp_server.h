//
// Created by behruz on 14.06.2021.
//

#ifndef FILE_EXCHANGE_UDP_SERVER_H
#define FILE_EXCHANGE_UDP_SERVER_H

#include "read_file.h"

struct udp_server_answer {
    int8_t success;
    uint16_t port;
    struct file_triplet_dto triplet;
};

void *start_udp_server(void *thread_data);

#endif //FILE_EXCHANGE_UDP_SERVER_H
