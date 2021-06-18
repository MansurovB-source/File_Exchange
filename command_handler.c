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


static int parse(const char *cmd, char **triplet) {
    const char *p = cmd;
    int count = 0;

    for (;;) {
        while (isspace(*p)) p++;
        if (count >= 4) {
            return count;
        }
        if (*p == '\0') break;

        if (*p == '"' || *p == '\'') {
            int quote = *p++;
            const char *begin = p;

            while (*p && *p != quote) { p++; }
            if (*p == '\0') {
                perror("Unmatched quote");
                return -1;
            }
            strncpy(triplet[count], begin, p - begin);
            count++;
            p++;
            continue;
        }

        if (isalnum(*p) || *p == '.' || *p == '/') {
            const char *begin = p;

            while (isalnum(*p) || *p == '.' || *p == '/') p++;
            strncpy(triplet[count], begin, p - begin);
            count++;
            continue;
        }

        perror("Illegal character");
        return -1;
    }

    return count;
}

void display_cmd(struct list *l, const char *file_name) {
    struct list *node = l;
    while (node != NULL) {
        if (!strcmp(node->value->filename, file_name)) {
            triplet_print(node->value);
            printf("\n");

        }
        node = node->next;
    }
}

void help_cmd() {
    printf("display [file_name]\n");
    printf("download [file_triplet]\n");
    printf("help\n");
    printf("exit\n");
}

// TODO
void download_cmd(const char *triplet) {
    char *triplet_str = malloc(512);
    strcpy(triplet_str, triplet);
    pthread_t *search_udp = (pthread_t *) malloc(sizeof(pthread_t));
    pthread_create(search_udp, NULL, search_server_udp, triplet_str);


}


int8_t handler_cmd(struct context *ctx, const char *cmd) {
    char code = 0;
    char *triplet[4];

    triplet[0] = calloc(1, 256);
    triplet[1] = calloc(1, 256);
    triplet[2] = calloc(1, 256);
    triplet[3] = calloc(1, 256);

    parse(cmd, triplet);

    if (!strcmp(triplet[0], DISPLAY_CMD)) {
        display_cmd(ctx->l, triplet[1]);
    } else if (!strcmp(triplet[0], DOWNLOAD_CMD)) {
        download_cmd(triplet[1]);
    } else if (!strcmp(triplet[0], HELP_CMD)) {
        help_cmd();
    } else if (!strcmp(triplet[0], EXIT_CMD)) {
        code = 1;
    } else {
        printf("Unknown command\n");
    }

    free(triplet[0]);
    free(triplet[1]);
    free(triplet[2]);
    free(triplet[3]);

    return code;
}
