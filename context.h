//
// Created by behruz on 16.06.2021.
//

#ifndef FILE_EXCHANGE_CONTEXT_H
#define FILE_EXCHANGE_CONTEXT_H

#include "list.h"


struct context {
    struct list *l;
    int8_t exit;
};

#endif //FILE_EXCHANGE_CONTEXT_H
