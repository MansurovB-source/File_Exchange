//
// Created by behruz on 17.06.2021.
//

#include <stdio.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <stddef.h>
#include <unistd.h>
#include <fcntl.h>
#include "tcp_server.h"
#include "tcp_client.h"

void serve_client(int socket_fd, struct file_triplet *triplet, struct context *ctx) {
//    char relative_path[256] = {0};
//    sprintf(relative_path, "./%s", triplet->filepath);
    put_action(ctx->events_module, triplet->filefullpath);
    int current_file = open(triplet->filefullpath, O_RDONLY, 00666);
    struct tcp_server_request request = {0};
    struct tcp_server_answer answer = {0};
    struct transfer_progress progress = {0};
    struct transfer_progress *cur_progress = &progress;
    struct list *existing_upload = find_upload(ctx->events_module, cur_progress);
    if (existing_upload) {
        cur_progress = existing_upload->value;
    } else {
        progress.triplet.filesize = triplet->filesize;
        strncpy(progress.triplet.hash, triplet->hash, 32);
        strcpy(progress.triplet.filename, triplet->filename);
        put_upload(ctx->events_module, &progress);
    }
    while (0 != strncmp("ext", request.cmd, 3)) {
        read(socket_fd, &request, sizeof(request));
        if (0 == strncmp("get", request.cmd, 3)) {
            uint16_t size = 4096;
            if (triplet->filesize < size * request.arg + 4096) {
                size = triplet->filesize - size * request.arg;
            }
            pread(current_file, answer.payload, size, 4096 * request.arg);
            answer.len = size;
            write(socket_fd, &answer, sizeof(answer));
        } else if (0 == strncmp("prg", request.cmd, 3)) {
            cur_progress->transferred = request.arg;
            put_upload(ctx->events_module, cur_progress);
        }
    }
    del_upload(ctx->events_module, cur_progress);
    close(current_file);
}

void init_tcp_server(struct tcp_server_data *server_data) {
    server_data->sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_data->sockfd == -1) {
        log_error(server_data->ctx->events_module, "[ERROR, TCP-server] TCP socket creation failed (%d)");
        return;
    }

    memset(&server_data->servaddr, 0, sizeof(struct sockaddr_in));

    uint16_t port_test = 2421;

    server_data->servaddr.sin_family = AF_INET;
    server_data->servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_data->servaddr.sin_port = htons(port_test);

    while (bind(server_data->sockfd, (const struct sockaddr *) &server_data->servaddr,
                sizeof(struct sockaddr_in)) < 0) {
        port_test++;
        server_data->servaddr.sin_port = htons(port_test);
    }
    server_data->port = port_test;
}


void *start_tcp_server(void *thread_data) {
    struct tcp_server_data *server_data = thread_data;
    int32_t connect_fd, len;

    if ((listen(server_data->sockfd, 5)) != 0) {
        log_error(server_data->ctx->events_module, "[TCP-server] {ERROR } Listen failed (%d)");
        return NULL;
    }
    len = sizeof(server_data->client);

    connect_fd = accept(server_data->sockfd, (struct sockaddr *) &server_data->client, &len);
    if (connect_fd < 0) {
        log_error(server_data->ctx->events_module, "[ERROR, TCP-server] Server accept failed (%d)");
        return NULL;
    }

    log_action(server_data->ctx->events_module, "Started uploading file %s", server_data->triplet->filename);
    serve_client(connect_fd, server_data->triplet, server_data->ctx);
    log_action(server_data->ctx->events_module, "Uploading is finished file %s", server_data->triplet->filename);

    close(server_data->sockfd);
    free(server_data);
    return NULL;
}