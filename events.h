//
// Created by behruz on 18.06.2021.
//

#ifndef FILE_EXCHANGE_EVENTS_H
#define FILE_EXCHANGE_EVENTS_H

#include "list.h"
#include <pthread.h>

struct progress_transfer {
    size_t transferred;
    struct file_triplet_dto triplet;
};

struct events_data {
    pthread_mutex_t upload_mutex;
    pthread_mutex_t download_mutex;
    pthread_mutex_t actions_mutex;
    struct list *upload_list;
    struct list *download_list;
    struct list *actions_list;
};

void init_events(struct events_data *events_data);

void destroy_events(struct events_data *events_data);

void put_action(struct events_data *events_data, char *str);

void put_download(struct events_data *events_data, struct progress_transfer *transfer_progress);

void del_download(struct events_data *events_data, struct progress_transfer *transfer_progress);

void put_upload(struct events_data *events_data, struct progress_transfer *transfer_progress);

void del_upload(struct events_data *events_data, struct progress_transfer *transfer_progress);

#endif //FILE_EXCHANGE_EVENTS_H
