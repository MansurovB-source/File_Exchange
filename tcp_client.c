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
#include "events.h"


const static char *GET = "get";
const static char *EXIT = "exit";
const static char *PROGRESS = "prg";


static void perform_download(int socket_fd, struct file_triplet_dto tripletDto, struct context *ctx) {
    struct progress_transfer progress = {0};
    struct tcp_server_request request = {0};
    struct tcp_server_answer answer = {0};

    struct progress_transfer *cur_progress = &progress;

    struct list *exist_download = find_download(ctx->events, &progress);
    if (exist_download) {
        cur_progress = exist_download->value;
    } else {
        put_download(ctx->events, &progress);
    }
    progress.triplet = tripletDto;
    put_download(ctx->events, &progress);

    int file = open(tripletDto.filename, O_WRONLY | O_CREAT, 00777);
    int counter = 0;

    while (cur_progress->global * 4096 < tripletDto.filesize) {
        strncpy(request.command, GET, 3);
        request.arg = cur_progress->global++;
        if (request.arg * 4096 > tripletDto.filesize) {
            break;
        }
        write(socket_fd, &request, sizeof(request));
        read(socket_fd, &answer, sizeof(answer));
        //printf("[TCP_CLIENT] received data, len: %d\n", answer.len);
        cur_progress->transferred += answer.len;
        put_download(ctx->events, cur_progress);
        pwrite(file, &answer.payload, answer.len, request.arg * 4096);

        strncpy(request.command, PROGRESS, 3);
        request.arg = cur_progress->transferred;
        write(socket_fd, &request, sizeof(request));
    }
    del_download(ctx->events, cur_progress);
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

    char *start_download = malloc(256);
    strcat(start_download, "Start downloading the file ");
    strcat(start_download, client_data->triplet_dto.filename);
    put_action(client_data->ctx->events, start_download);
    perform_download(socket_fd, client_data->triplet_dto, client_data->ctx);

    char *finish_download = malloc(256);
    strcat(finish_download, "Finished downloading the file ");
    strcat(finish_download, client_data->triplet_dto.filename);
    put_action(client_data->ctx->events, finish_download);

    close(socket_fd);
    free(client_data);
    return NULL;
}