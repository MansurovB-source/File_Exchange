//
// Created by behruz on 19.06.2021.
//

#include "ui_module.h"
#include <ncurses.h>
#include <string.h>
#include "context.h"
#include "command_handler.h"

void render_transfer_area(int start_y, int end_y, int start_x, int end_x, char *str, struct list *l) {
    mvprintw(start_y, start_x, str);

    for (int i = 0; i < end_x; i++) {
        mvprintw(start_y + 1, i + start_x, "_");
    }

    for (int i = 0; i < end_x; i++) {
        mvprintw(end_y, i + start_x, "_");
    }

    for (int i = start_y + 2; i <= end_y; i++) {
        mvprintw(i, end_x, "|");
    }

    int transfer_start = start_y + 2;

    for (int i = 0; i < end_y - start_y - 2; i++) {
        struct list *current = get(l, i);
        if (current) {
            struct progress_transfer *progress = current->value;
            mvprintw(transfer_start + i, start_x, progress->triplet.filename);
            size_t name_len = strlen(progress->triplet.filename);
            mvprintw(transfer_start + i, start_x + name_len + 1, "%f/%fMB|[xxx...]",
                     progress->transferred / 1024.0 / 1024.0,
                     progress->triplet.filesize / 1024.0 / 1024.0);
        } else {
            break;
        }
    }
}

void render_log_area(int start, int end, int cols, struct list *l) {
    for (int i = 0; i < cols; i++) {
        mvprintw(start + 1, i, "_");
    }

    mvprintw(start, 0, "Actions/events log");

    for (int32_t i = 0; i < cols; i++) {
        mvprintw(end, i, "_");
    }

    int log_start = start + 2;

    for (int i = 0; i < end - start - 2; i++) {
        struct list *current = get(l, i);
        if (current) {
            mvprintw(log_start + i, 0, current->value);
        } else {
            break;
        }
    }
}

render_input_field(int32_t start) {
    mvprintw(start, 0, "> ");
}

void render(struct context *ctx) {
    int y;
    int x;
    getmaxyx(stdscr, y, x);

    int transfer_area_start = 0;
    int transfer_area_end = y / 2;

    int log_area_start = y / 2 + 1;
    int log_area_end = y - 2;

    int input_area = y - 1;

    clear();
    render_transfer_area(transfer_area_start, transfer_area_end, 0, x / 2, "DOWNLOAD", ctx->events->download_list);
    render_transfer_area(transfer_area_start, transfer_area_end, x / 2 + 1, x, "UPLOAD", ctx->events->upload_list);
    render_log_area(log_area_start, log_area_end, x, ctx->events->actions_list);
    render_input_field(input_area);

}

void launch(struct context *ctx) {
    //initscr();
    char exit = 0;

    while (!exit) {
        char *str = malloc(256);
        render(ctx);
        mvprintw(LINES - 1, 0, "> ");
        printf("HEllo");
        getstr(str);
        exit = handler_cmd(ctx, str);
    }
    endwin();
}