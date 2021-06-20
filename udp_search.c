//
// Created by behruz on 12.06.2021.
//

#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include "udp_search.h"
#include "udp_server.h"
#include "tcp_client.h"

#define PORT 8080
#define BUFFER_SIZE 2048

void *search_server_udp(void *data) {
    struct udp_client_data *udp_client_data = data;
    char *triplet = udp_client_data->triplet_str;
    int socket_fd;
    if ((socket_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        log_error(udp_client_data->ctx->events, "[UDP SEARCH]: {ERROR} Socket creation failed (%d)");
        return NULL;
    }

    struct sockaddr_in server_address;
    struct sockaddr_in client_address;
    memset(&server_address, 0, sizeof(struct sockaddr_in));
    memset(&client_address, 0, sizeof(struct sockaddr_in));

    int broadcast = 1;

    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;

    if (setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
        log_error(udp_client_data->ctx->events, "[UDP SEARCH]: {ERROR} couldn't set receive timeout (%d)");
        return NULL;
    }
    setsockopt(socket_fd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast));

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = htonl(INADDR_BROADCAST);

    size_t n;
    size_t server_address_size = sizeof(server_address);
    size_t client_address_size = sizeof(client_address);

    if ((sendto(socket_fd, triplet, strlen(triplet), 0, (const struct sockaddr *) &server_address,
                server_address_size)) < 0) {
        log_error(udp_client_data->ctx->events, "[UDP SEARCH]: {ERROR} Can't send data (%d)");
        return NULL;
    }

    char buffer[BUFFER_SIZE] = {0};

    int8_t received_smth = 0;

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        if ((n = recvfrom(socket_fd, (char *) buffer, BUFFER_SIZE,
                          MSG_WAITALL, (struct sockaddr *) &client_address,
                          &client_address_size)) == -1) {
            if (received_smth) {
                return NULL;
            }
            put_action(udp_client_data->ctx->events, "[UDP SEARCH]: {ERROR} File not found");
            break;
        }
        received_smth = 1;

        buffer[n] = '\0';

        struct udp_server_answer *answer = (struct udp_server_answer *) buffer;

        if (answer->success) {
            pthread_t *tcp_client = (pthread_t *) malloc(sizeof(pthread_t));;
            struct tcp_client_data *tcp_client_data = malloc(sizeof(struct tcp_client_data));
            tcp_client_data->port = answer->port;
            tcp_client_data->triplet_dto = answer->triplet;
            tcp_client_data->server_address = client_address.sin_addr.s_addr;
            tcp_client_data->ctx = udp_client_data->ctx;
            pthread_create(tcp_client, NULL, start_client_tcp, tcp_client_data);
        }
    }

    close(socket_fd);
    free(triplet);
    free(udp_client_data);

    return NULL;
}