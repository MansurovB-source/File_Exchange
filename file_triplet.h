//
// Created by behruz on 12.06.2021.
//

#ifndef FILE_EXCHANGE_FILE_TRIPLET_H
#define FILE_EXCHANGE_FILE_TRIPLET_H

#include <stdint.h>
#include <openssl/sha.h>
#include <malloc.h>
#include "list.h"


struct file_triplet {
    char *filename;
    uint64_t filesize;
    char hash[SHA256_DIGEST_LENGTH * 2];
    char *filepath;
};

struct file_triplet_dto {
    char filename[256];
    uint64_t filesize;
    char hash[SHA256_DIGEST_LENGTH * 2];
};

void file_triplet_destroy(void *data);
void calc_hash(FILE *file, uint8_t *hash);
void triplet_print(struct file_triplet *triplet);
struct file_triplet *find_triplet(struct list *l, char *triplet);
#endif //FILE_EXCHANGE_FILE_TRIPLET_H
