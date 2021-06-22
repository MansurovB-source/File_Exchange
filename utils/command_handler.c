//
// Created by behruz on 12.06.2021.
//

#include <malloc.h>
#include <stdint.h>
#include <string.h>
#include <bits/pthreadtypes.h>
#include <pthread.h>
#include "command_handler.h"
#include "util.h"
#include "dir_reader/read_file.h"
#include "../network/udp_search.h"

const static char *DISPLAY_CMD = "display";
const static char *DOWNLOAD_CMD = "download";
const static char *EXIT_CMD = "exit";
const static char *HELP_CMD = "help";

const static char *unknown_cmd = "unknown cmd";

const static char *help_display = "display [filename]";
const static char *help_download = "download [file triplet]";
const static char *help_exit = "display [filename]";
const static char *help_help = "exit [filename]";

void display_cmd(struct context *ctx, const char *path) {
    struct list *element = ctx->triplet_list;
    while (element != NULL) {
        struct file_triplet *triplet = ((struct file_triplet *) element->value);
        if (!strcmp(triplet->filepath, path)) {
            char *triplet_str = malloc(256);
            char *filesize_str = malloc(16);

            memset(triplet_str, 0, 256);
            memset(triplet_str, 0, 16);

            strcat(triplet_str, triplet->filename);
            strcat(triplet_str, ":");
            sprintf(filesize_str, "%ld", triplet->filesize);
            strcat(triplet_str, filesize_str);
            strcat(triplet_str, ":");
            strncat(triplet_str, triplet->hash, 32);

            put_action(ctx->events_module, triplet_str);
            free(triplet_str);
            free(filesize_str);
            return;
        }
        element = element->next;
    }
    log_action(ctx->events_module, "Couldn't find file %s", path);
}

void download_cmd(char *triplet, struct context *ctx) {
    char *triplet_str = calloc(1, 512);
    memset(triplet_str, 0, 512);

    strcpy(triplet_str, triplet);
    struct udp_client_data *udp_cd = malloc(sizeof(struct udp_client_data));
    udp_cd->triplet_str = triplet_str;
    udp_cd->ctx = ctx;
    pthread_t *search_udp = (pthread_t *) malloc(sizeof(pthread_t));
    pthread_create(search_udp, NULL, search_udp_servers, udp_cd);
}

void help_cmd(struct context *ctx) {
    put_action(ctx->events_module, help_display);
    put_action(ctx->events_module, help_download);
    put_action(ctx->events_module, help_exit);
    put_action(ctx->events_module, help_help);
}

/** non-zero value == terminate program */
int8_t handle_command(struct context *ctx, const char *cmd) {
    int8_t ret_code = 0;
    char *args[3];
    args[0] = malloc(256);
    args[1] = malloc(256);
    args[2] = malloc(256);

    memset(args[0], 0, 256);
    memset(args[1], 0, 256);
    memset(args[2], 0, 256);

    parse(cmd, args);

    if (!strcmp(args[0], DISPLAY_CMD)) {
        display_cmd(ctx, args[1]);
    } else if (!strcmp(args[0], DOWNLOAD_CMD)) {
        download_cmd(args[1], ctx);
    } else if (!strcmp(args[0], HELP_CMD)) {
        help_cmd(ctx);
    } else if (!strcmp(args[0], EXIT_CMD)) {
        ctx->exit = 1;
        ret_code = 1;
    } else {
        put_action(ctx->events_module, unknown_cmd);
    }

    free(args[0]);
    free(args[1]);
    free(args[2]);

    return ret_code;
}