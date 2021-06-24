//
// Created by behruz on 18.06.2021.
//

#ifndef FILE_EXCHANGE_EVENTS_H
#define FILE_EXCHANGE_EVENTS_H


#include <pthread.h>
#include "../utils/list/list.h"
#include "../utils/dir_reader/read_file.h"

struct transfer_progress {
    size_t transferred;
    struct file_triplet_dto triplet;
    _Atomic size_t global;
};

struct events_data {
    pthread_mutex_t upload_mutex;
    pthread_mutex_t download_mutex;
    pthread_mutex_t actions_mutex;
    struct list *upload_list;
    struct list *download_list;
    struct list *actions_list;
    void *ui_data;
};

void init_events_module(struct events_data *events_data);

void destroy_events_module(struct events_data *events_data);

struct list *find_download(struct events_data *events_data, struct transfer_progress *transfer_progress);

struct list *find_upload(struct events_data *events_data, struct transfer_progress *transferProgress);

void put_download(struct events_data *events_data, struct transfer_progress *transfer_progress);

void del_download(struct events_data *events_data, struct transfer_progress *transfer_progress);

void put_upload(struct events_data *events_data, struct transfer_progress *transfer_progress);

void del_upload(struct events_data *events_data, struct transfer_progress *transferProgress);

void put_action(struct events_data *events_data, const char *str);

void log_error(struct events_data *events_data, char *msg);

void log_action(struct events_data *events_data, const char *msg, const char *arg);

#endif //FILE_EXCHANGE_EVENTS_H
