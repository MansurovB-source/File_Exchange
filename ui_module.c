//
// Created by behruz on 19.06.2021.
//

#include "ui_module.h"
#include <ncurses.h>
#include <string.h>
#include <unistd.h>
#include "context.h"
#include "command_handler.h"


void focus_input(struct ui_data *data) {
    int x, y;
    getyx(data->input_win, y, x);
    wmove(data->input_win, y, x);
}

void render_transfer_area(struct ui_data *data, char do_clear) {
    int rows;
    int cols;

    getmaxyx(data->transfer_win, rows, cols);
    if (do_clear) {
        wclear(data->transfer_win);
        box(data->transfer_win, 0, 0);
        wmove(data->transfer_win, 0, 0);
        wprintw(data->transfer_win, "Downloading");
        wmove(data->transfer_win, 0, cols / 2);
        wprintw(data->transfer_win, "Uploading");
        for (int32_t i = 0; i < rows; i++) {
            wmove(data->transfer_win, i, cols / 2 - 1);
            wprintw(data->transfer_win, "|");
        }
    }

    int available_rows = cols - 2;
    int transfer_start = 1;
    for (int i = 0; i < available_rows; i++) {
        struct list *current = get(data->ctx->events->download_list, i);
        if (current) {
            struct progress_transfer *progress = current->value;
            size_t name_len = strlen(progress->triplet.filename);
            wmove(data->transfer_win, transfer_start + i, 1);
            wprintw(data->transfer_win, progress->triplet.filename);
            wmove(data->transfer_win, transfer_start + i, name_len + 2);
            float transferred = progress->transferred / 1024.0 / 1024.0;
            float all = progress->triplet.filesize / 1024.0 / 1024.0;
            wprintw(data->transfer_win, "%f/%fMB|[", transferred, all);

            float percent = transferred / all;
            int32_t x = percent * 10;
            int32_t dot = 10 - x;

            for (int j = 0; j < x; ++j) {
                wprintw(data->transfer_win, "x");
            }
            for (int j = 0; j < dot; ++j) {
                wprintw(data->transfer_win, ".");
            }
            wprintw(data->transfer_win, "]");
        } else {
            break;
        }
    }

    for (int i = 0; i < available_rows; i++) {
        struct list *current = get(data->ctx->events->upload_list, i);
        if (current) {
            struct progress_transfer *progress = current->value;
            size_t name_len = strlen(progress->triplet.filename);
            wmove(data->transfer_win, transfer_start + i, cols / 2 + 2);
            wprintw(data->transfer_win, progress->triplet.filename);
            wmove(data->transfer_win, transfer_start + i, cols / 2 + 2 + name_len + 1);
            float transferred = progress->transferred / 1024.0 / 1024.0;
            float all = progress->triplet.filesize / 1024.0 / 1024.0;
            wprintw(data->transfer_win, "%f/%fMB|[", transferred, all);

            float percent = transferred / all;
            int32_t x = percent * 10;
            int32_t dot = 10 - x;

            for (int j = 0; j < x; ++j) {
                wprintw(data->transfer_win, "x");
            }
            for (int j = 0; j < dot; ++j) {
                wprintw(data->transfer_win, ".");
            }
            wprintw(data->transfer_win, "]");
        } else {
            break;
        }
    }
    wrefresh(data->transfer_win);
    focus_input(data);
}

void render_log_area(struct ui_data *data, char do_clear) {
    int rows, cols;
    getmaxyx(data->events_win, rows, cols);

    if (do_clear) {
        wclear(data->events_win);
        box(data->events_win, 0, 0);
        wmove(data->events_win, 1, 1);
        wprintw(data->events_win, "Actions/events log");
    }

    int32_t available_rows = rows - 3;
    int32_t log_start = 2;
    for (int32_t i = 0; i < available_rows; i++) {
        struct list *current = get(data->ctx->events->actions_list, i);
        if (current) {
            wmove(data->events_win, log_start + i, 1);
            wprintw(data->events_win, current->value);
        } else {
            break;
        }
    }
    wrefresh(data->events_win);
    focus_input(data);
}

void render_input_field(struct ui_data *data) {
    wclear(data->input_win);
    wprintw(data->input_win, "> ");
    wrefresh(data->input_win);
    focus_input(data);
}

void init_ui_data(struct ui_data *data) {
    initscr();
    curs_set(0);
    refresh();

    int row, col;
    getmaxyx(stdscr, row, col);

    int transfer_area_start = 0;
    int transfer_area_end = row / 2 - 1;
    int events_log_start = row / 2 - 1;
    int events_log_end = row - 1;
    int input_start = row - 1;

    data->transfer_win = newwin(transfer_area_end - transfer_area_start, COLS, transfer_area_start, 0);
    data->events_win = newwin(events_log_end - events_log_start, COLS, events_log_start, 0);
    data->input_win = newwin(1, COLS, input_start, 0);
}

void destroy_ui_data(struct ui_data *data) {
    delwin(data->transfer_win);
    delwin(data->events_win);
    delwin(data->input_win);
}

void render_screen(struct ui_data *ui_data) {
    render_log_area(ui_data, 1);
    render_input_field(ui_data);
}


void launch(struct ui_data *data) {
    printf("Hello_1");
    render_transfer_area(data, 1);
    render_log_area(data, 1);
    render_input_field(data);
    char exit = 0;
    while (!exit) {
        char *str = malloc(256);
        usleep(1000 * 50);
        render_screen(data);
        wgetstr(data->input_win, str);
        exit = handler_cmd(data->ctx, str);
        free(str);
    }
    destroy_ui_data(data);
    endwin();
}