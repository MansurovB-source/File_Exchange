//
// Created by behruz on 12.06.2021.
//

#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <malloc.h>
#include <pthread.h>
#include "command_handler.h"
#include "list.h"
#include "udp_search.h"


const static char *DISPLAY_CMD = "display";
const static char *DOWNLOAD_CMD = "download";
const static char *EXIT_CMD = "exit";
const static char *HELP_CMD = "help";
const static char *UNKNOWN_COMMAND = "unknown_cmd";

int parse(const char *cmd, char **args) {
    const char *p = cmd;
    int count = 0;

    for (;;) {
        while (isspace(*p)) p++;
        if (count >= 3) {
            return count;
        }
        if (*p == '\0') break;

        if (*p == '"' || *p == '\'') {
            int quote = *p++;
            const char *begin = p;

            while (*p && *p != quote) p++;
            if (*p == '\0') return -1;
            strncpy(args[count], begin, p - begin);
            count++;
            p++;
            continue;
        }

        if (strchr("<>()|", *p)) {
            args[count] = calloc(1, 256);
            strncpy(args[count], p, 1);
            count++;
            p++;
            continue;
        }

        if (isalnum(*p) || *p == '.' || *p == '/' || *p == '-' || *p == ':') {
            const char *begin = p;

            while (isalnum(*p) || *p == '.' || *p == '/' || *p == '-' || *p == ':') p++;
            strncpy(args[count], begin, p - begin);
            count++;
            continue;
        }

        return -1;
    }

    return count;
}

void display_cmd(struct context *ctx, const char *path) {
    struct list *node = ctx->l;

    while (node->value != NULL) {
        struct file_triplet *triplet = (struct file_triplet *) node->value;
        if (!strcmp(triplet->filepath, path)) {
            char *triplet_str = malloc(256);
            char *filesize_str = malloc(16);

            strcat(triplet_str, triplet->filename);
            strcat(triplet_str, ":");
            sprintf(filesize_str, "%ld", triplet->filesize);
            strcat(triplet_str, filesize_str);
            strcat(triplet_str, ":");
            strncat(triplet_str, triplet->hash, 32);

            put_action(ctx->events, triplet_str);
            free(triplet_str);
            free(filesize_str);
            return;
        }
        node = node->next;
    }
    log_action(ctx->events, "Could find file %s", path);
}

void help_cmd(struct context *ctx) {
    put_action(ctx->events, "display [file_name]");
    put_action(ctx->events, "download [file_triplet]");
    put_action(ctx->events, "help");
    put_action(ctx->events, "exit");
}

// TODO
void download_cmd(const char *triplet, struct context *ctx) {
    char *triplet_str = malloc(512);
    strcpy(triplet_str, triplet);
    struct udp_client_data *udp_client_data = malloc(sizeof(struct udp_client_data));
    udp_client_data->triplet_str = triplet_str;
    udp_client_data->ctx = ctx;

    pthread_t *search_udp = (pthread_t *) malloc(sizeof(pthread_t));
    pthread_create(search_udp, NULL, search_server_udp, udp_client_data);
}


int8_t handler_cmd(struct context *ctx, const char *cmd) {
    char code = 0;
    char *triplet[4];

    triplet[0] = malloc(256);
    triplet[1] = malloc(256);
    triplet[2] = malloc(256);
    triplet[3] = malloc(256);

    parse(cmd, triplet);

    if (!strcmp(triplet[0], DISPLAY_CMD)) {
        display_cmd(ctx, triplet[1]);
    } else if (!strcmp(triplet[0], DOWNLOAD_CMD)) {
        download_cmd(triplet[1], ctx);
    } else if (!strncmp(triplet[0], HELP_CMD, 4)) {
        help_cmd(ctx);
    } else if (!strncmp(triplet[0], EXIT_CMD, 4)) {
        code = 1;
        ctx->exit = 1;
    } else {
        put_action(ctx->events, triplet[0]);
    }

    free(triplet[0]);
    free(triplet[1]);
    free(triplet[2]);
    free(triplet[3]);

    return code;
}
