//
// Created by behruz on 09.06.2021.
//

#ifndef FILE_EXCHANGE_LIST_H
#define FILE_EXCHANGE_LIST_H

#include "malloc.h"
#include "file_triplet.h"
typedef struct file_triplet *entry;

struct list {
    entry value;
    struct list *next;
};

void list_add_front(struct list **l, entry e);
void list_add_back(struct list **l, entry e);
void list_destroy(struct list *l);
void list_print(struct list *l);

#endif //FILE_EXCHANGE_LIST_H
