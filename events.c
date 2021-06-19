//
// Created by behruz on 18.06.2021.
//

#include <string.h>
#include "events.h"

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

    events_data->upload_list = malloc(sizeof(struct list));
    events_data->download_list = malloc(sizeof(struct list));
    events_data->actions_list = malloc(sizeof(struct list));
}

void destroy_events(struct events_data *events_data) {
    pthread_mutex_destroy(&events_data->upload_mutex);
    pthread_mutex_destroy(&events_data->download_mutex);
    pthread_mutex_destroy(&events_data->actions_mutex);

    list_destroy(events_data->upload_list, destroy_transfer_progress);
    list_destroy(events_data->upload_list, destroy_transfer_progress);
    list_destroy(events_data->upload_list, destroy_message_actions);
}

static struct list *fine_transfer_progress(struct list *transfer_list, struct progress_transfer *transfer_progress) {
    struct list *tmp = transfer_list;
    while (tmp != NULL) {
        struct progress_transfer *tmp_value = tmp->value;
        if (tmp_value
            && 0 == strcmp(tmp_value->triplet.filename, transfer_progress->triplet.filename)
            && 0 == strcmp(tmp_value->triplet.hash, transfer_progress->triplet.hash)) {
            return tmp;
        }
        tmp = tmp->next;
    }
    return NULL;
}

static void time_tag(char *str) {
    time_t timer = time(NULL);
    struct tm* info;
    info = localtime(&timer);
    strftime(str, 29, "[%H:%M:%S %Y-%m-%d] ", info);
}

void put_action(struct events_data *events_data, char *str) {
    pthread_mutex_lock(&events_data->actions_mutex);
    char *log = malloc(512);
    time_tag(log);
    strcat(log, str);
    list_add_front(&events_data->actions_list, log);
    pthread_mutex_unlock(&events_data->actions_mutex);
}

void put_download(struct events_data *events_data, struct progress_transfer *transfer_progress) {
    pthread_mutex_lock(&events_data->download_mutex);
    struct list *found = fine_transfer_progress(events_data->download_list, transfer_progress);
    if (found) {
        struct progress_transfer *data = found->value;
        data->transferred = transfer_progress->transferred;
    } else {
        list_add_front(&events_data->download_list, transfer_progress);
    }
    pthread_mutex_unlock(&events_data->download_mutex);
}

void del_download(struct events_data *events_data, struct progress_transfer *transfer_progress) {
    pthread_mutex_lock(&events_data->download_mutex);
    struct list *found = fine_transfer_progress(events_data->download_list, transfer_progress);
    if (found) {
        //TODO check
        events_data->download_list = remove_element(&events_data->download_list, found, destroy_transfer_progress);
        if (!events_data->download_list) {
            printf("[EVENTS]: {ERROR} On deleting download");
        }
    } else {
        printf("[EVENTS]: {ERROR} On deleting download");

    }
    pthread_mutex_unlock(&events_data->download_mutex);
}

void put_upload(struct events_data *events_data, struct progress_transfer *transfer_progress) {
    pthread_mutex_lock(&events_data->upload_mutex);
    struct list *found = fine_transfer_progress(events_data->upload_list, transfer_progress);
    if (found) {
        struct progress_transfer *data = found->value;
        data->transferred = transfer_progress->transferred;
    } else {
        list_add_front(&events_data->upload_list, transfer_progress);
    }
    pthread_mutex_unlock(&events_data->upload_mutex);
}

void del_upload(struct events_data *events_data, struct progress_transfer *transfer_progress) {
    pthread_mutex_lock(&events_data->upload_mutex);
    struct list *found = fine_transfer_progress(events_data->upload_list, transfer_progress);
    if (found) {
        //TODO check
        events_data->upload_list = remove_element(&events_data->upload_list, found, destroy_transfer_progress);
        if (!events_data->upload_list) {
            printf("[EVENTS]: {ERROR} On deleting upload");
        }
    } else {
        printf("[EVENTS]: {ERROR} On deleting upload");

    }
    pthread_mutex_unlock(&events_data->upload_mutex);
}