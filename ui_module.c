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
    wrefresh(data->input_win);
}

void render_transfer_headers(struct ui_data *data) {
    mvprintw(0, 1, "Downloading");
    mvprintw(0, COLS / 2 + 1, "Uploading");
    refresh();
}

void render_download_area(struct ui_data *data, char do_clear) {
    int rows, cols;
    getmaxyx(data->download_win, rows, cols);
    if (do_clear) {
        wclear(data->download_win);
        box(data->download_win, 0, 0);
    }
    int available_rows = cols - 2;
    int transfer_start = 1;
    for (int i = 0; i < available_rows; i++) {
        struct list *current = get(data->ctx->events->download_list, i);
        if (current) {
            struct progress_transfer *progress = current->value;
            size_t name_len = strlen(progress->triplet.filename);
            wmove(data->download_win, transfer_start + i, 1);
            wprintw(data->download_win, progress->triplet.filename);
            wmove(data->download_win, transfer_start + i, name_len + 2);
            float transferred = progress->transferred / 1024.0 / 1024.0;
            float all = progress->triplet.filesize / 1024.0 / 1024.0;
            wprintw(data->download_win, "%f/%fMB|[", transferred, all);

            float percent = transferred / all;
            int x = (percent * 10);
            int dot = 10 - x;

            for (int j = 0; j < x; ++j) {
                wprintw(data->download_win, "x");
            }
            for (int j = 0; j < dot; ++j) {
                wprintw(data->download_win, ".");
            }
            wprintw(data->download_win, "]");
        } else {
            break;
        }
    }
    wrefresh(data->download_win);
}

void render_upload_area(struct ui_data *data, char do_clear) {
    int rows, cols;
    getmaxyx(data->upload_win, rows, cols);
    if (do_clear) {
        wclear(data->upload_win);
        box(data->upload_win, 0, 0);
    }
    int available_rows = cols - 2;
    int transfer_start = 1;
    for (int i = 0; i < available_rows; i++) {
        struct list *current = get(data->ctx->events->upload_list, i);
        if (current) {
            struct progress_transfer *progress = current->value;
            size_t name_len = strlen(progress->triplet.filename);
            wmove(data->upload_win, transfer_start + i, 1);
            wprintw(data->upload_win, progress->triplet.filename);
            wmove(data->upload_win, transfer_start + i, (int) name_len + 2);
            float transferred = progress->transferred / 1024.0 / 1024.0;
            float all = progress->triplet.filesize / 1024.0 / 1024.0;
            wprintw(data->upload_win, "%f/%fMB|[", transferred, all);

            float percent = transferred / all;
            int x = (int) (percent * 10);
            int dot = 10 - x;

            for (int j = 0; j < x; ++j) {
                wprintw(data->upload_win, "x");
            }
            for (int j = 0; j < dot; ++j) {
                wprintw(data->upload_win, ".");
            }
            wprintw(data->upload_win, "]");
        } else {
            break;
        }
    }
    wrefresh(data->upload_win);
}

void render_transfer_area(struct ui_data *data, char do_clear) {
    render_download_area(data, do_clear);
    render_upload_area(data, do_clear);
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

    int transfer_area_start = 1;
    int transfer_area_end = row / 2 - 1;
    int events_log_start = row / 2 - 1;
    int events_log_end = row - 1;
    int input_start = row - 1;

    data->download_win = newwin(transfer_area_end - transfer_area_start, col / 2, transfer_area_start, 0);
    data->upload_win = newwin(transfer_area_end - transfer_area_start, col / 2 + col % 2, transfer_area_start, col / 2);
    data->events_win = newwin(events_log_end - events_log_start, col, events_log_start, 0);
    data->input_win = newwin(1, col, input_start, 0);
}

void destroy_ui_data(struct ui_data *data) {
    delwin(data->download_win);
    delwin(data->upload_win);
    delwin(data->events_win);
    delwin(data->input_win);
}

void render_all(struct ui_data *data) {
    render_log_area(data, 1);
    render_transfer_headers(data);
    render_transfer_area(data, 1);
    render_input_field(data);
}


void launch(struct ui_data *data) {
    render_all(data);
    char exit = 0;
    while (!exit) {
        char *str = malloc(256);
        usleep(1000 * 50);
        render_input_field(data);
        curs_set(1);
        wgetstr(data->input_win, str);
        exit = handler_cmd(data->ctx, str);
        free(str);
    }
    destroy_ui_data(data);
    endwin();
}