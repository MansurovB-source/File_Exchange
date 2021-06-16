//
// Created by behruz on 17.06.2021.
//

#ifndef FILE_EXCHANGE_TCP_CLIENT_H
#define FILE_EXCHANGE_TCP_CLIENT_H

struct tcp_server_answer {
    int16_t len;
    int8_t payload[4096];
};

void *start_client_tcp(void *data);

#endif //FILE_EXCHANGE_TCP_CLIENT_H
