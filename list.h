//
// Created by behruz on 09.06.2021.
//

#ifndef FILE_EXCHANGE_LIST_H
#define FILE_EXCHANGE_LIST_H

#include <stddef.h>

struct list {
    void *value;
    struct list *next;
};

struct list *get(struct list *l, size_t index);

struct list *push(struct list *l, void *value);

struct list *remove_el(struct list *l, struct list *element);

void destroy_list(struct list *l, int (*destroy_data)(void *));

#endif //FILE_EXCHANGE_LIST_H
