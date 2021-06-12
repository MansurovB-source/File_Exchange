//
// Created by behruz on 12.06.2021.
//

#ifndef FILE_EXCHANGE_FILE_TRIPLET_H
#define FILE_EXCHANGE_FILE_TRIPLET_H

#include <stdint.h>
#include <openssl/sha.h>
#include <malloc.h>


struct file_triplet {
    char *filename;
    uint64_t filesize;
    uint8_t hash[SHA256_DIGEST_LENGTH];
};

void file_triplet_destroy(struct file_triplet *triplet);
void calc_hash(FILE *file, uint8_t *hash);
void triplet_print(struct file_triplet *triplet);

#endif //FILE_EXCHANGE_FILE_TRIPLET_H
