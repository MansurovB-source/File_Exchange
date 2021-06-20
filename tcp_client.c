//
// Created by behruz on 17.06.2021.
//


#include <netinet/in.h>
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
    progress.triplet = tripletDto;

    struct list *exist_download = find_download(ctx->events, &progress);
    if (exist_download) {
        cur_progress = exist_download->value;
    } else {
        put_download(ctx->events, &progress);
    }
    int file = open(tripletDto.filename, O_WRONLY | O_CREAT, 00777);

    while (cur_progress->global * 4096 < tripletDto.filesize) {
        strncpy(request.command, GET, 3);
        request.arg = cur_progress->global++;
        if (request.arg * 4096 > tripletDto.filesize) {
            break;
        }
        write(socket_fd, &request, sizeof(request));
        read(socket_fd, &answer, sizeof(answer));
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
}

void *start_client_tcp(void *data) {
    struct tcp_client_data *client_data = data;
    int socket_fd;

    struct sockaddr_in server_address;

    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        log_error(client_data->ctx->events, "[TCP CLIENT]: {ERROR} Socket creation failed (%d)");
        return NULL;
    }

    memset(&server_address, 0, sizeof(struct sockaddr_in));

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = client_data->server_address;
    server_address.sin_port = htons(client_data->port);

    if (connect(socket_fd, (struct sockaddr *) &server_address, sizeof(server_address)) != 0) {
        log_error(client_data->ctx->events, "[TCP CLIENT]: {ERROR} Connection with the server failed (%d)");
        return NULL;
    }

    log_action(client_data->ctx->events, "Started downloading file %s", client_data->triplet_dto.filename);
    perform_download(socket_fd, client_data->triplet_dto, client_data->ctx);

    log_action(client_data->ctx->events, "Finished downloading file %s", client_data->triplet_dto.filename);

    close(socket_fd);
    free(client_data);
    return NULL;
}