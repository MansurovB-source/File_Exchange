//
// Created by behruz on 12.06.2021.
//

#ifndef FILE_EXCHANGE_COMMAND_HANDLER_H
#define FILE_EXCHANGE_COMMAND_HANDLER_H

#include "list.h"
#include <stdint.h>

int8_t handler_cmd(struct list *l, const char *cmd);
void download_cmd(struct list *l, const char *triplet);
void help_cmd();
#endif //FILE_EXCHANGE_COMMAND_HANDLER_H
