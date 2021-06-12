//
// Created by behruz on 12.06.2021.
//

#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <malloc.h>
#include "command_handler.h"


const static char *DISPLAY_CMD = "display";
const static char *DOWNLOAD_CMD = "download";
const static char *EXIT_CMD = "exit";

int parse(const char *cmd, char **triplet) {
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
            list_print(node);
            printf("\n");
        }
        node = node->next;
    }
}

// TODO
void download_cmd(struct list *l, const char *triplet);


int8_t cmd_handler(struct list *l, const char *cmd) {
    char code = 0;
    char *triplet[4];

    triplet[0] = calloc(1, 256);
    triplet[1] = calloc(1, 256);
    triplet[2] = calloc(1, 256);
    triplet[3] = calloc(1, 256);

    parse(cmd, triplet);

    if (!strcmp(triplet[0], DISPLAY_CMD)) {
        display_cmd(l, triplet[1]);
    } else if (!strcmp(triplet[0], DOWNLOAD_CMD)) {
        download_cmd(l, triplet[1]);
    } else if (!strcmp(triplet[0], EXIT_CMD)) {
        code = 1;
    }

    free(triplet[0]);
    free(triplet[1]);
    free(triplet[2]);
    free(triplet[3]);

    return code;
}


cha

