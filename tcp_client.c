//
// Created by behruz on 17.06.2021.
//

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "tcp_client.h"
#include "tcp_server.h"

const static char *GET = "get";
const static char *EXIT = "exit";

static void perform_download(int socket_fd, struct file_triplet_dto tripletDto) {
    struct tcp_server_request request = {0};
    struct tcp_server_answer answer = {0};
    int file = open(tripletDto.filename, O_WRONLY | O_CREAT, 00777);
    int counter = 0;
    for(size_t i = 0; i < tripletDto.filesize; i += 4096) {
        strncpy(request.command, GET, 4);
        request.arg = 0;
        write(socket_fd, &request, sizeof(request));
        read(socket_fd, &answer, sizeof(answer));
        printf("[TCP_CLIENT] received data, len: %d\n", answer.len);
        pwrite(file, &answer.payload, answer.len, i);
        counter++;
    }
    close(file);
    strncpy(request.command, EXIT, 4);
    write(socket_fd, &request, sizeof(request));
    //printf("Client Exit...\n");
}

void *start_client_tcp(void *data) {
    struct tcp_client_data *client_data = data;
    int socket_fd;

    struct sockaddr_in server_address;

    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("[TCP CLIENT]: {ERROR} socket creation failed...");
        exit(0);
    } else {
        printf("[TCP CLIENT]: Socket successfully created...\n");
    }

    memset(&server_address, 0, sizeof(struct sockaddr_in));

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = client_data->server_address;
    server_address.sin_port = htons(client_data->port);

    if (connect(socket_fd, (struct sockaddr *) &server_address, sizeof(server_address)) != 0) {
        perror("[TCP CLIENT]: {ERROR} Connection with the server failed...");
        exit(0);
    } else
        printf("[TCP CLIENT]: Connected to the server...\n");

    perform_download(socket_fd, client_data->triplet_dto);

    close(socket_fd);
    free(client_data);
    return NULL;
}