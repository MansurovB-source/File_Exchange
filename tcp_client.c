//
// Created by behruz on 17.06.2021.
//

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "tcp_client.h"
#include "tcp_server.h"

static void client_message(int socket_fd) {
    struct tcp_server_request request = {0};
    char get[4] = "get";
    char exit[4] = "exit";
    strncpy(request.command, get, 4);
    write(socket_fd, &request, sizeof(request));
    strncpy(request.command, exit, 4);
    write(socket_fd, &request, sizeof(request));
    printf("Client Exit...\n");
}

void *start_client_tcp(void *data) {
    struct tcp_server_data *server_data = data;
    int socket_fd;

    struct sockaddr_in server_address;

    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("TCP: socket creation failed...");
        exit(0);
    } else {
        printf("TCP: Socket successfully created...\n");
    }

    memset(&server_address, 0, sizeof(struct sockaddr_in));

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_address.sin_port = htons(server_data->udp_server_ans.port);

    if (connect(socket_fd, (struct sockaddr *) &server_address, sizeof(server_address)) != 0) {
        perror("TCP: Connection with the server failed...");
        exit(0);
    } else
        printf("TCP: Connected to the server...\n");

    client_message(socket_fd);

    close(socket_fd);
}