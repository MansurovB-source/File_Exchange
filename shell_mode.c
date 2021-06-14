//
// Created by behruz on 14.06.2021.
//

#include "shell_mode.h"

char *get_line(void) {
    char *line = malloc(256);
    char *ptr = line;
    size_t len_max = 100;
    size_t len = len_max;
    int c;

    if (line == NULL)
        return NULL;

    for (;;) {
        c = fgetc(stdin);
        if (c == EOF)
            break;

        if (--len == 0) {
            len = len_max;
            char *linen = realloc(ptr, len_max *= 2);

            if (linen == NULL) {
                free(ptr);
                return NULL;
            }
            line = linen + (line - ptr);
            ptr = linen;
        }

        if ((*line++ = c) == '\n')
            break;
    }
    *(line-1) = '\0';
    return ptr;
}

void shell(struct list *l) {
    char exit = 0;
    while ((!exit)) {
        printf("> ");
        char *line = get_line();
        exit = handler_cmd(l, line);
        free(line);
    }
}