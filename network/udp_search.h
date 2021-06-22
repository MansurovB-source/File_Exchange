//
// Created by behruz on 12.06.2021.
//

#ifndef FILE_EXCHANGE_UDP_SEARCH_H
#define FILE_EXCHANGE_UDP_SEARCH_H

#include "../utils/context.h"

struct udp_client_data {
    char *triplet_str;
    struct context *ctx;
};

void *search_udp_servers(void *thread_data);

#endif //FILE_EXCHANGE_UDP_SEARCH_H
