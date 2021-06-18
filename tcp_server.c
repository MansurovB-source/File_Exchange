//
// Created by behruz on 17.06.2021.
//

#include <stdio.h>
#include "tcp_server.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "tcp_client.h"


static void conversation(int socket_fd, struct file_triplet *triplet) {
    int file = open(triplet->filename, O_RDONLY, 00666);
    struct tcp_server_request request;
    struct tcp_server_answer answer;

    while (strcmp("exit", request.command)) {
        read(socket_fd, &request, sizeof(request));
        printf("[TCP SERVER]: From client: %s\t to server: ", request.command);
        if (0 == strncmp("get", request.command, 3)) {
            uint16_t size = 4096;
            if (triplet->filesize < size * request.arg + 4096) {
                size = triplet->filesize - size * request.arg;
            }
            pread(file, answer.payload, size, 4096 * request.arg);
            answer.len = size;
            write(socket_fd, &answer, sizeof(answer));
        }
    }
    close(file);
    printf("TCP: Server exit... \n");
}

void init_server_tcp(struct tcp_server_data *server_data) {
    if ((server_data->socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("[TCP SERVER]: {ERROR} Socket creation failed...");
        return;
    } else {
        printf("[TCP SERVER]: Socket successfully created... \n");
    }

    memset(&server_data->server_address, 0, sizeof(struct sockaddr_in));
    uint16_t port = 4455;

    server_data->server_address.sin_family = AF_INET;
    server_data->server_address.sin_port = htons(port);
    server_data->server_address.sin_addr.s_addr = htonl(INADDR_ANY);

    while ((bind(server_data->socket_fd, (struct sockaddr *) &server_data->server_address,
                 sizeof(struct sockaddr_in))) < 0) {
        //perror("TCP: Binding is failed...");
        port++;
        server_data->server_address.sin_port = htons(port);
    }
    server_data->port = port;
    printf("[TCP SERVER]: Socket successfully binded..\n");
}

void *start_server_tcp(void *data) {
    struct tcp_server_data *server_data = data;
    int32_t connect_fd;

    if ((listen(server_data->socket_fd, 5)) != 0) {
        printf("[TCP SERVER]: {ERROR} Listen failed...\n");
        exit(0);
    } else {
        printf("[TCP SERVER]: Server listening...\n");
    }
    size_t client_address_size = sizeof(server_data->client_address);

    if ((connect_fd = accept(server_data->socket_fd, (struct sockaddr *) &server_data->client_address,
                (socklen_t *) &client_address_size)) < 0) {
        perror("[TCP SERVER]: {ERROR} Server accept failed...");
        exit(0);
    }
    printf("[TCP SERVER]: {ERROR} Server accept the client...\n");

    conversation(connect_fd, server_data->triplet);
    close(connect_fd);
    free(server_data);
    return NULL;
}