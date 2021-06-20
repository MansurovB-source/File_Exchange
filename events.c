//
// Created by behruz on 18.06.2021.
//

#include <string.h>
#include <errno.h>
#include "events.h"
#include "ui_module.h"

static void destroy_transfer_progress(void *data) {
    struct progress_transfer *progress_transfer = data;
    free(progress_transfer);
}

static void destroy_message_actions(void *data) {
    char *str = data;
    free(str);

}

void init_events(struct events_data *events_data) {
    pthread_mutex_init(&events_data->upload_mutex, NULL);
    pthread_mutex_init(&events_data->download_mutex, NULL);
    pthread_mutex_init(&events_data->actions_mutex, NULL);

    events_data->upload_list = NULL; // malloc(sizeof(struct list));
    events_data->download_list = NULL; // malloc(sizeof(struct list));
    events_data->actions_list = NULL; //malloc(sizeof(struct list));
}

void destroy_events(struct events_data *events_data) {
    pthread_mutex_destroy(&events_data->upload_mutex);
    pthread_mutex_destroy(&events_data->download_mutex);
    pthread_mutex_destroy(&events_data->actions_mutex);

    list_destroy(events_data->upload_list, destroy_transfer_progress);
    list_destroy(events_data->download_list, destroy_transfer_progress);
    list_destroy(events_data->actions_list, destroy_message_actions);
}

static struct list *find_transfer_progress(struct list *transfer_list, struct progress_transfer *transfer_progress) {
    struct list *tmp = transfer_list;
    while (tmp != NULL && transfer_progress != NULL) {
        struct progress_transfer *tmp_value = tmp->value;
        if (tmp_value
            && 0 == strcmp(tmp_value->triplet.filename, transfer_progress->triplet.filename)
            && 0 == strncmp(tmp_value->triplet.hash, transfer_progress->triplet.hash, 32)) {
            return tmp;
        }
        tmp = tmp->next;
    }
    return NULL;
}

static void time_tag(char *str) {
    time_t timer = time(NULL);
    struct tm *info;
    info = localtime(&timer);
    strftime(str, 29, "[%H:%M:%S %Y-%m-%d] ", info);
}

void put_action(struct events_data *events_data, char *str) {
    pthread_mutex_lock(&events_data->actions_mutex);
    char *log = malloc(512);
    time_tag(log);
    strcat(log, str);
    list_add_front(&events_data->actions_list, log);
    render_log_area(events_data->ui_data, 1);
    pthread_mutex_unlock(&events_data->actions_mutex);
}

void put_download(struct events_data *events_data, struct progress_transfer *transfer_progress) {
    pthread_mutex_lock(&events_data->download_mutex);
    struct list *found = find_transfer_progress(events_data->download_list, transfer_progress);
    char do_clear = 0;
    if (found) {
        struct progress_transfer *data = found->value;
        data->transferred = transfer_progress->transferred;
    } else {
        list_add_front(&events_data->download_list, transfer_progress);
        do_clear = 1;
    }
    render_transfer_area(events_data->ui_data, do_clear);
    pthread_mutex_unlock(&events_data->download_mutex);
}

void del_download(struct events_data *events_data, struct progress_transfer *transfer_progress) {
    pthread_mutex_lock(&events_data->download_mutex);
    struct list *found = find_transfer_progress(events_data->download_list, transfer_progress);
    if (found) {
        //TODO check
        events_data->download_list = remove_element(&events_data->download_list, found, destroy_transfer_progress);
        if (!events_data->download_list) {
            put_action(events_data, "[EVENTS]: {ERROR} On deleting download");
        }
    }
    render_transfer_area(events_data->ui_data, 1);
    pthread_mutex_unlock(&events_data->download_mutex);
}

void put_upload(struct events_data *events_data, struct progress_transfer *transfer_progress) {
    pthread_mutex_lock(&events_data->upload_mutex);
    struct list *found = find_transfer_progress(events_data->upload_list, transfer_progress);
    char do_clear = 0;
    if (found) {
        struct progress_transfer *data = found->value;
        data->transferred = transfer_progress->transferred;
    } else {
        list_add_front(&events_data->upload_list, transfer_progress);
        do_clear = 1;
    }
    render_transfer_area(events_data->ui_data, do_clear);
    pthread_mutex_unlock(&events_data->upload_mutex);
}

void del_upload(struct events_data *events_data, struct progress_transfer *transfer_progress) {
    pthread_mutex_lock(&events_data->upload_mutex);
    struct list *found = find_transfer_progress(events_data->upload_list, transfer_progress);
    if (found) {
        //TODO check
        events_data->upload_list = remove_element(&events_data->upload_list, found, destroy_transfer_progress);
        if (!events_data->upload_list) {
            put_action(events_data, "[EVENTS]: {ERROR} On deleting upload");
        }
    }
    render_transfer_area(events_data->ui_data, 1);
    pthread_mutex_unlock(&events_data->upload_mutex);
}

struct list *find_download(struct events_data *events_data, struct progress_transfer *progress) {
    pthread_mutex_lock(&events_data->download_mutex);
    struct list *cur = find_transfer_progress(events_data->download_list, progress);
    pthread_mutex_unlock(&events_data->download_mutex);
    return cur;
}

struct list *find_upload(struct events_data *events_data, struct progress_transfer *progress) {
    pthread_mutex_lock(&events_data->upload_mutex);
    struct list *cur = find_transfer_progress(events_data->upload_list, progress);
    pthread_mutex_unlock(&events_data->upload_mutex);
    return cur;
}

void log_error(struct events_data *events_data, char *msg) {
    char error[256] = {0};
    sprintf(error, msg, errno);
    put_action(events_data, error);
}

void log_action(struct events_data *events_data, const char *msg, const char *arg) {
    char action[256] = {0};
    sprintf(action, msg, arg);
    put_action(events_data, action);
}