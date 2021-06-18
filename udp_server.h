//
// Created by behruz on 14.06.2021.
//

#ifndef FILE_EXCHANGE_UDP_SERVER_H
#define FILE_EXCHANGE_UDP_SERVER_H

#include <stdint.h>
#include "stdio.h"
#include "file_triplet.h"

struct udp_server_answer {
    uint8_t success;
    uint16_t port;
    //TODO struct file_triplet_dto;
    struct file_triplet_dto triplet;
};

void *start_server_udp(void *data);

#endif //FILE_EXCHANGE_UDP_SERVER_H
