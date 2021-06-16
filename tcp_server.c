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


static void conversation(int socket_fd) {
    struct tcp_server_request request;

    while (strcmp("exit", request.command)) {
        read(socket_fd, &request, sizeof(request));
        printf("TCP: From client: %s\t to server: ", request.command);
    }
    printf("TCP: Server exit... \n");
}

void *start_server_tcp(void *data) {
    struct tcp_server_data *server_data = data;
    int socket_fd;

    struct sockaddr_in server_address;
    struct sockaddr_in client_address;


    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("TCP: Socket creation failed...");
        exit(0);
    } else {
        printf("TCP: Socket successfully created... \n");
    }

    memset(&server_address, 0, sizeof(struct sockaddr_in));
    memset(&client_address, 0, sizeof(struct sockaddr_in));

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(server_data->udp_server_ans.port);
    // TODO
    server_address.sin_addr.s_addr = INADDR_ANY;

    if ((bind(socket_fd, (struct sockaddr *) &server_address, sizeof(server_address))) != 0) {
        perror("TCP: Binding is failed...");
        exit(0);
    }

    printf("TCP: Server listening...\n");

    size_t client_address_size = sizeof(client_address);

    if ((accept(socket_fd, (struct sockaddr *) &client_address, (socklen_t *) &client_address_size)) < 0) {
        perror("TCP: Server accept failed...");
        exit(0);
    }

    printf("TCP: Server accept the client...\n");

    conversation(socket_fd);

    close(socket_fd);
    return NULL;
}