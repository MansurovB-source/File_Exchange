//
// Created by behruz on 16.06.2021.
//

#ifndef FILE_EXCHANGE_CONTEXT_H
#define FILE_EXCHANGE_CONTEXT_H

#include <stdint.h>
#include "list/list.h"
#include "../event/events.h"

struct context {
    struct list *triplet_list;
    struct events_data *events_module;
    int8_t exit;
};

#endif //FILE_EXCHANGE_CONTEXT_H
