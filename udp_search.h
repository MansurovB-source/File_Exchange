//
// Created by behruz on 12.06.2021.
//

#ifndef FILE_EXCHANGE_UDP_SEARCH_H
#define FILE_EXCHANGE_UDP_SEARCH_H

#include "context.h"

void *search_server_udp(void *data);

struct udp_client_data {
    char *triplet_str;
    struct context *ctx;
};

#endif //FILE_EXCHANGE_UDP_SEARCH_H
