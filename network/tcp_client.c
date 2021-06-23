//
// Created by behruz on 17.06.2021.
//

#include "tcp_client.h"
#include "tcp_server.h"
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>

const static char *GET_CMD = "get";
const static char *PROGRESS_CMD = "prg";
const static char *EXIT_CMD = "ext";

void perform_download(int socket_fd, struct file_triplet_dto triplet, struct context *ctx) {
    struct transfer_progress progress = {0};
    struct tcp_server_request request = {0};
    struct tcp_server_answer answer = {0};
    struct transfer_progress *cur_progress = &progress;
    progress.triplet = triplet;
    struct list *existing_download = find_download(ctx->events_module, &progress);
    if (existing_download) {
        cur_progress = existing_download->value;
    } else {
        put_download(ctx->events_module, &progress);
    }
    int current_file = open(triplet.filename, O_WRONLY | O_CREAT, 00777);
    while (cur_progress->global * 4096 < triplet.filesize) {
        strncpy(request.cmd, GET_CMD, 3);
        request.arg = cur_progress->global++;
        if (request.arg * 4096 > triplet.filesize) {
            break;
        }
        write(socket_fd, &request, sizeof(request));
        read(socket_fd, &answer, sizeof(answer));
        cur_progress->transferred += answer.len;
        put_download(ctx->events_module, cur_progress);
        pwrite(current_file, &answer.payload, answer.len, request.arg * 4096);
        // send progress status to the server
        strncpy(request.cmd, PROGRESS_CMD, 3);
        request.arg = cur_progress->transferred;
        write(socket_fd, &request, sizeof(request));
    }
    del_download(ctx->events_module, cur_progress);
    close(current_file);
    strncpy(request.cmd, EXIT_CMD, 3);
    write(socket_fd, &request, sizeof(request));
}

void *start_tcp_client(void *thread_data) {
    struct tcp_client_data *client_data = thread_data;
    int socket_fd;
    struct sockaddr_in server_address;

    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1) {
        log_error(client_data->ctx->events_module, "[TCP_CLIENT] {ERROR} socket creation failed (%d)");
        return NULL;
    }
    memset(&server_address, 0, sizeof(server_address));

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = client_data->server_addr;
    server_address.sin_port = htons(client_data->port);

    if (connect(socket_fd, (struct sockaddr *) &server_address, sizeof(server_address)) != 0) {
        log_error(client_data->ctx->events_module, "[TCP_CLIENT] {ERROR} connection with the server failed (%d)");
        return NULL;
    }

    log_action(client_data->ctx->events_module, "Started downloading file %s", client_data->triplet.filename);
    perform_download(socket_fd, client_data->triplet, client_data->ctx);
    log_action(client_data->ctx->events_module, "Finished downloading file %s", client_data->triplet.filename);;

    close(socket_fd);
    free(client_data);
    return NULL;
}

//download "progit.pdf:17763840:CD5246703E9742BFBDD56C18F9985199"
//download "util.c:11499:9182245C29B08F589E2B6970380EBC85"
