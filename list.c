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

//TODO check
struct list *remove_element(struct list **l, struct list *element, void (*destroy_entry) (void *)) {
    struct list *tmp = *l;
    struct list *tmp_prev;
    if(tmp == element) {
        *l = tmp->next;
        destroy_entry(tmp->value);
        free(tmp);
        return *l;
    } else {
        while(tmp != NULL) {
            if(tmp == element) {
                tmp_prev->next = element->next;
                destroy_entry(tmp->value);
                free(tmp);
                return *l;
            }
            tmp_prev = tmp;
            tmp = tmp->next;
        }
    }
    return NULL;
}

static struct list *list_last(struct list *l) {
    if (l == NULL) { return NULL; }
    struct list *cur = l;
    for (; cur->next != NULL; cur = cur->next) {
    }
    return cur;
}

void list_add_back(struct list **l, entry e) {
    if (*l) {
        list_last(*l)->next = node_create(e);
    } else {
        list_add_front(l, e);
    }
}

void list_destroy(struct list *l, void (*destroy_entry)(void *)) {
    if (!l) {
        return;
    }
    struct list *cur = l;
    while (cur) {
        struct list *next = cur->next;
        destroy_entry(cur->value);

        free(cur);
        cur = next;
    }
}
