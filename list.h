//
// Created by behruz on 09.06.2021.
//

#ifndef FILE_EXCHANGE_LIST_H
#define FILE_EXCHANGE_LIST_H

#include "malloc.h"
#include "file_triplet.h"

typedef void *entry;

struct list {
    entry value;
    struct list *next;
};

void list_add_front(struct list **l, entry e);

void list_add_back(struct list **l, entry e);

void list_destroy(struct list *l, void (*destroy_entry)(void *));

struct list *remove_element(struct list **l, struct list *element, void (*destroy_entry)(void *));

#endif //FILE_EXCHANGE_LIST_H
