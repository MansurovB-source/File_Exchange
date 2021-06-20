//
// Created by behruz on 17.06.2021.
//

#include "tcp_server.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "tcp_client.h"
#include "events.h"


static void serv_client(int socket_fd, struct file_triplet *triplet, struct context *ctx) {
    char relative_path[256] = {0};
    sprintf(relative_path, "./%s", triplet->filepath);
    int file = open(relative_path, O_RDONLY, 00666);
    struct tcp_server_request request = {0};
    struct tcp_server_answer answer = {0};
    struct progress_transfer progress = {0};
    struct progress_transfer *cur_progress = &progress;
    struct list *exist_upload = find_upload(ctx->events, cur_progress);
    if (exist_upload) {
        cur_progress = exist_upload->value;
    } else {
        progress.triplet.filesize = triplet->filesize;
        strncpy(progress.triplet.hash, triplet->hash, 32);
        strcpy(progress.triplet.filename, triplet->filename);
        put_upload(ctx->events, &progress);
    }
    while (strncmp("exit", request.command, 4) != 0) {
        read(socket_fd, &request, sizeof(request));
        if (0 == strncmp("get", request.command, 3)) {
            uint16_t size = 4096;
            if (triplet->filesize < size * request.arg + 4096) {
                size = triplet->filesize - size * request.arg;
            }
            pread(file, answer.payload, size, 4096 * request.arg);
            answer.len = size;
            write(socket_fd, &answer, sizeof(answer));
        } else if (0 == strncmp("prg", request.command, 3)) {
            cur_progress->transferred = request.arg;
            put_upload(ctx->events, cur_progress);
        }
    }
    del_upload(ctx->events, cur_progress);
    close(file);
}

void init_server_tcp(struct tcp_server_data *server_data) {
    if ((server_data->socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        log_error(server_data->ctx->events, "[TCP SERVER]: {ERROR} Socket creation failed (%d)");
        return;
    }

    memset(&server_data->server_address, 0, sizeof(struct sockaddr_in));
    uint16_t port = 4455;

    server_data->server_address.sin_family = AF_INET;
    server_data->server_address.sin_port = htons(port);
    server_data->server_address.sin_addr.s_addr = htonl(INADDR_ANY);

    while ((bind(server_data->socket_fd, (struct sockaddr *) &server_data->server_address,
                 sizeof(struct sockaddr_in))) < 0) {
        port++;
        server_data->server_address.sin_port = htons(port);
    }
    server_data->port = port;
}

void *start_server_tcp(void *data) {
    struct tcp_server_data *server_data = data;
    int32_t connect_fd;

    if ((listen(server_data->socket_fd, 5)) != 0) {
        log_error(server_data->ctx->events, "[TCP SERVER]: {ERROR} Listen failed (%d)");
        return NULL;
    }

    size_t client_address_size = sizeof(server_data->client_address);

    if ((connect_fd = accept(server_data->socket_fd, (struct sockaddr *) &server_data->client_address,
                             (socklen_t *) &client_address_size)) < 0) {
        log_error(server_data->ctx->events, "[TCP SERVER]: {ERROR} Server accept failed (%d)");
        return NULL;
    }

    log_action(server_data->ctx->events, "Started uploading file %s", server_data->triplet->filename);

    serv_client(connect_fd, server_data->triplet, server_data->ctx);
    log_action(server_data->ctx->events, "Upload finished file %s", server_data->triplet->filename);

    close(connect_fd);
    free(server_data);
    return NULL;
}