//
// Created by behruz on 09.06.2021.
//

#include "list.h"

static struct list *node_create(entry e) {
    struct list *result = (struct list *) malloc(sizeof(struct list));
    result->value = e;
    result->next = NULL;
    return result;
}

void list_add_front(struct list **l, entry e) {
    if (*l) {
        struct list *const old_start = *l;
        *l = node_create(e);
        (*l)->next = old_start;
    } else {
        *l = node_create(e);
    }
}

static struct list *list_last(struct list *l) {
    if (l == NULL) { return NULL; }
    struct list *cur = l;
    for (; cur->next != NULL; cur = cur->next) {
    }
    return cur;
}

static void free_node(entry e) {
    free(e->filename);
    free(e);
}

void list_add_back(struct list **l, entry e) {
    if (*l) {
        list_last(*l)->next = node_create(e);
    } else {
        list_add_front(l, e);
    }
}

void list_destroy(struct list *l) {
    if (!l) {
        return;
    }
    struct list *cur = l;
    while (cur) {
        struct list *next = cur->next;
        free_node(cur->value);
        free(cur);
        cur = next;
    }
}

void node_print(struct list *l) {
    printf(" %s", l->value->filename);
    printf(" %lu", l->value->filesize);
    printf(" %lu", (uint64_t) l->value->hash);
}