//
// Created by behruz on 19.06.2021.
//

#ifndef FILE_EXCHANGE_UI_MODULE_H
#define FILE_EXCHANGE_UI_MODULE_H

#include <ncurses.h>
#include "context.h"

struct ui_data {
    WINDOW *transfer_win;
    WINDOW *events_win;
    WINDOW *input_win;
    struct context *ctx;
};

void launch(struct ui_data *data);

void render_screen(struct ui_data *ui_data);

void destroy_ui_data(struct ui_data *data);

void init_ui_data(struct ui_data *data);

void render_input_field(struct ui_data *data);

void render_log_area(struct ui_data *data, char do_clear);

void render_transfer_area(struct ui_data *data, char do_clear);

#endif //FILE_EXCHANGE_UI_MODULE_H