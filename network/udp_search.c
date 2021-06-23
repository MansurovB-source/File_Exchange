//
// Created by behruz on 12.06.2021.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <arpa/inet.h>

#include "udp_search.h"
#include "udp_server.h"
#include "tcp_client.h"

#define PORT 8080
#define BUF_SIZE 1024

void *search_udp_servers(void *thread_data) {
    struct udp_client_data *udp_cd = thread_data;
    char *triplet_str = udp_cd->triplet_str;

    int socket_fd;
    int8_t buffer[BUF_SIZE] = {0};
    struct sockaddr_in server_address, client_address;

    if ((socket_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        log_error(udp_cd->ctx->events_module, "[UDP-search] {ERROR} Couldn't create socket (%d)");
        return NULL;
    }

    int broadcast = 1;
    setsockopt(socket_fd, SOL_SOCKET, SO_BROADCAST,
               &broadcast, sizeof broadcast);

    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;

    if (setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
        log_error(udp_cd->ctx->events_module, "[UDP-search] {ERROR} Couldn't set receive timeout (%d)");
        return NULL;
    }


    memset(&server_address, 0, sizeof(server_address));
    memset(&client_address, 0, sizeof(client_address));

    server_address.sin_family = AF_INET;
    inet_pton(AF_INET, "255.255.255.255", &server_address.sin_addr);
    server_address.sin_port = htons(PORT);
    //server_address.sin_addr.s_addr = inet_addr("172.30.255.255");
    //server_address.sin_addr.s_addr = htonl(INADDR_BROADCAST);

    size_t n, len;

    len = sizeof(client_address);

    size_t c = sendto(socket_fd, triplet_str, strlen(triplet_str),
                      0, (const struct sockaddr *) &server_address,
                      sizeof(server_address));
    if (c < 0) {
        log_error(udp_cd->ctx->events_module, "[UDP-search] {ERROR} (%d)");
        return NULL;
    }

    int8_t received_smth = 0;

    while (1) {
        memset(buffer, 0, BUF_SIZE);
        n = recvfrom(socket_fd, (char *) buffer, BUF_SIZE,
                     MSG_WAITALL, (struct sockaddr *) &client_address,
                     (socklen_t *) &len);

        if (-1 == n) {
            if (received_smth) {
                return NULL;
            }
            put_action(udp_cd->ctx->events_module, "[UDP-search] file not found");
            break;
        }

        put_action(udp_cd->ctx->events_module, "[UDP SEARCH] server");
        received_smth = 1;

        buffer[n] = '\0';

        struct udp_server_answer *answer = (struct udp_server_answer *) buffer;

        if (answer->success) {
            pthread_t *tcp_client = (pthread_t *) malloc(sizeof(pthread_t));
            struct tcp_client_data *tcp_cd = malloc(sizeof(struct tcp_client_data));
            tcp_cd->port = answer->port;
            tcp_cd->triplet = answer->triplet;
            tcp_cd->server_addr = client_address.sin_addr.s_addr;
            tcp_cd->ctx = udp_cd->ctx;
            pthread_create(tcp_client, NULL, start_tcp_client, tcp_cd);
        }
    }

    close(socket_fd);
    free(triplet_str);
    free(udp_cd);

    return NULL;
}