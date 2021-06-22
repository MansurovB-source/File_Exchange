//
// Created by behruz on 09.06.2021.
//

#ifndef FILE_EXCHANGE_READ_FILE_H
#define FILE_EXCHANGE_READ_FILE_H

#include <stdint.h>
#include "../list/list.h"

#include <stdint.h>
#include <openssl/sha.h>
#include "../list/list.h"

struct file_triplet {
    char *filename;
    char hash[SHA256_DIGEST_LENGTH * 2];
    size_t filesize;
    char *filepath;
};

struct file_triplet_dto {
    char filename[256];
    char hash[SHA256_DIGEST_LENGTH * 2];
    size_t filesize;
};

int8_t run_file_reader(char *dir_path, struct list **p_l);

void destroy_file_triplet(struct file_triplet *triplet);

#endif //FILE_EXCHANGE_READ_FILE_H
