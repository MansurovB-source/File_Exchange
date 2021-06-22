//
// Created by behruz on 18.06.2021.
//

#include <malloc.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include "events.h"
#include "../ui/ui_module.h"


static struct list *find_transfer_progress(struct list *transfer_list, struct transfer_progress *transfer_progress) {
    struct list *element = transfer_list;
    while (element != NULL && transfer_progress != NULL) {
        struct transfer_progress *element_value = element->value;
        if (element_value &&
            0 == strcmp(element_value->triplet.filename, transfer_progress->triplet.filename) &&
            0 == strncmp(element_value->triplet.hash, transfer_progress->triplet.hash, 32)
                ) {
            return element;
        }
        element = element->next;
    }
    return NULL;
}

void destroy_transfer_progress(struct transfer_progress *transfer_progress) {
    free(transfer_progress);
}

void destroy_action_message(char *str) {
    free(str);
}

struct list *find_download(struct events_data *events_data, struct transfer_progress *transfer_trogress) {
    pthread_mutex_lock(&events_data->download_mutex);
    struct list *element = find_transfer_progress(events_data->download_list, transfer_trogress);
    pthread_mutex_unlock(&events_data->download_mutex);
    return element;
}

struct list *find_upload(struct events_data *events_data, struct transfer_progress *transfer_progress) {
    pthread_mutex_lock(&events_data->upload_mutex);
    struct list *element = find_transfer_progress(events_data->upload_list, transfer_progress);
    pthread_mutex_unlock(&events_data->upload_mutex);
    return element;
}

void init_events_module(struct events_data *events_data) {
    pthread_mutex_init(&events_data->actions_mutex, NULL);
    pthread_mutex_init(&events_data->download_mutex, NULL);
    pthread_mutex_init(&events_data->upload_mutex, NULL);

    events_data->actions_list = malloc(sizeof(struct list));
    events_data->download_list = malloc(sizeof(struct list));
    events_data->upload_list = malloc(sizeof(struct list));

    memset(events_data->actions_list, 0, sizeof(struct list));
    memset(events_data->download_list, 0, sizeof(struct list));
    memset(events_data->upload_list, 0, sizeof(struct list));
}

void destroy_events_module(struct events_data *events_data) {
    pthread_mutex_destroy(&events_data->actions_mutex);
    pthread_mutex_destroy(&events_data->download_mutex);
    pthread_mutex_destroy(&events_data->upload_mutex);

    destroy_list(events_data->download_list, (int (*)(void *)) destroy_transfer_progress);
    destroy_list(events_data->upload_list, (int (*)(void *)) destroy_transfer_progress);
    destroy_list(events_data->actions_list, (int (*)(void *)) destroy_action_message);
}

void put_download(struct events_data *events_data, struct transfer_progress *transfer_progress) {
    pthread_mutex_lock(&events_data->download_mutex);
    struct list *found = find_transfer_progress(events_data->download_list, transfer_progress);
    int8_t do_clear = 0;
    if (found) {
        struct transfer_progress *value = found->value;
        value->transferred = transfer_progress->transferred;
    } else {
        events_data->download_list = push(events_data->download_list, transfer_progress);
        do_clear = 1;
    }
    render_transfer_area(events_data->ui_data, do_clear);
    pthread_mutex_unlock(&events_data->download_mutex);
}

void del_download(struct events_data *events_data, struct transfer_progress *transfer_progress) {
    pthread_mutex_lock(&events_data->download_mutex);
    struct list *found = find_transfer_progress(events_data->download_list, transfer_progress);
    if (found) {
        events_data->download_list = remove_el(events_data->download_list, found);
        if (!events_data->download_list) {
            put_action(events_data, "[EVENTS-MODULE] ERROR on del download");
        }
    }
    render_transfer_area(events_data->ui_data, 1);
    pthread_mutex_unlock(&events_data->download_mutex);
}

void put_upload(struct events_data *events_data, struct transfer_progress *transfer_progress) {
    pthread_mutex_lock(&events_data->upload_mutex);
    struct list *found = find_transfer_progress(events_data->upload_list, transfer_progress);
    int8_t do_clear = 0;
    if (found) {
        struct transfer_progress *value = found->value;
        value->transferred = transfer_progress->transferred;
    } else {
        events_data->upload_list = push(events_data->upload_list, transfer_progress);
        do_clear = 1;
    }
    render_transfer_area(events_data->ui_data, do_clear);
    pthread_mutex_unlock(&events_data->upload_mutex);
}

void del_upload(struct events_data *events_data, struct transfer_progress *transfer_progress) {
    pthread_mutex_lock(&events_data->upload_mutex);
    struct list *found = find_transfer_progress(events_data->upload_list, transfer_progress);
    if (found) {
        events_data->upload_list = remove_el(events_data->upload_list, found);
        if (!events_data->upload_list) {
            put_action(events_data, "[EVENTS-MODULE] ERROR on del upload");
        }
    }
    render_transfer_area(events_data->ui_data, 1);
    pthread_mutex_unlock(&events_data->upload_mutex);
}

void add_time_tag(char *str) {
    time_t timer;
    struct tm *tm_info;
    timer = time(NULL);
    tm_info = localtime(&timer);
    strftime(str, 29, "[%Y-%m-%d %H:%M:%S] ", tm_info);
}

void put_action(struct events_data *events_data, const char *str) {
    pthread_mutex_lock(&events_data->actions_mutex);
    char *logged_str = calloc(1, 512);
    add_time_tag(logged_str);
    strcat(logged_str, str);
    events_data->actions_list = push(events_data->actions_list, logged_str);
    render_events_log(events_data->ui_data, 1);
    pthread_mutex_unlock(&events_data->actions_mutex);
}

void log_error(struct events_data *em, char *msg) {
    char error[256] = {0};
    sprintf(error, msg, errno);
    put_action(em, error);
}

void log_action(struct events_data *em, const char *msg, const char *arg) {
    char action[256] = {0};
    sprintf(action, msg, arg);
    put_action(em, action);
}