//
// Created by behruz on 19.06.2021.
//

#ifndef FILE_EXCHANGE_UI_MODULE_H
#define FILE_EXCHANGE_UI_MODULE_H

#include "../utils/context.h"
#include <curses.h>

struct ui_data {
    WINDOW *download_win;
    WINDOW *upload_win;
    WINDOW *events_win;
    WINDOW *input_win;
    struct context *ctx;
};

void render_transfer_area(struct ui_data *data, int8_t do_clear);

void render_events_log(struct ui_data *data, int8_t do_clear);

void init_ui_data(struct ui_data *data);

void destroy_ui_data(struct ui_data *data);

void start_ui(struct ui_data *ui_data);

#endif //FILE_EXCHANGE_UI_MODULE_H