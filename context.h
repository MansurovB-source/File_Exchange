//
// Created by behruz on 16.06.2021.
//

#ifndef FILE_EXCHANGE_CONTEXT_H
#define FILE_EXCHANGE_CONTEXT_H

#include "list.h"
#include "events.h"


struct context {
    struct list *l;
    int8_t exit;
    struct events_data *events;
};

#endif //FILE_EXCHANGE_CONTEXT_H
