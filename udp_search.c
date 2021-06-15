//
// Created by behruz on 12.06.2021.
//

#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>
#include "udp_search.h"
#include "udp_server.h"

#define PORT 8080
#define BUFFER_SIZE 2048

void *search_server_udp(char *triplet) {
    int socket_fd;
    if ((socket_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Can't create socket");
        exit(-1);
    }

    struct sockaddr_in server_address;

    memset(&server_address, 0, sizeof(struct sockaddr_in));

    int broadcast = 1;
    setsockopt(socket_fd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast));

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = INADDR_ANY;

    int n;
    size_t server_address_size = sizeof(server_address);

    if ((sendto(socket_fd, triplet, strlen(triplet), 0, (const struct sockaddr *) &server_address,
                sizeof(server_address))) < 0) {
        perror("ERROR: Can't send data: ");
    }

    char buffer[BUFFER_SIZE];
    n = recvfrom(socket_fd, (char *) buffer, BUFFER_SIZE,
                 MSG_WAITALL, (struct sockaddr *) &server_address,
                 (socklen_t *) &server_address_size);
    buffer[n] = '\0';

    struct udp_server_answer *answer = (struct udp_server_answer *) buffer;

    //TODO tcp client

    close(socket_fd);

    return NULL;
}