//
// Created by behruz on 09.06.2021.
//

#include <malloc.h>
#include "list.h"

struct list *push(struct list *l, void *value) {
    struct list *element = malloc(sizeof(struct list));
    element->next = l;
    element->value = value;
    return element;
}

struct list *get(struct list *l, size_t index) {
    struct list *element = l;
    size_t i = 0;
    while (element != NULL) {
        if (element->value != NULL) {
            if (i == index) {
                return element;
            }
            i++;
        }
        element = element->next;
    }
    return NULL;
}

struct list *remove_el(struct list *l, struct list *element) {
    struct list *item = l;
    if (item == element) {
        return item->next;
    }
    while (item != NULL) {
        if (item->next == element) {
            item->next = element->next;
            element->next = NULL;
            return l;
        }
        item = item->next;
    }
    return NULL;
}

void destroy_list(struct list *l, int (*destroy_data)(void *)) {
    struct list *element = l;
    while (element != NULL) {
        if (element->value != NULL) {
            destroy_data(element->value);
        }
        struct list *prev = element;
        element = element->next;
        free(prev);
    }
}
