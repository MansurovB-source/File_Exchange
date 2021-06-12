//
// Created by behruz on 12.06.2021.
//

#include "file_triplet.h"

void file_triplet_destroy(struct file_triplet *triplet) {
    free(triplet->filename);
    free(triplet);
}

void calc_hash(FILE *file, uint8_t *hash) {
    SHA256_CTX sha256_ctx;
    SHA256_Init(&sha256_ctx);
    char data[1024];
    uint64_t bytes;
    while ((bytes = fread(data, 1, 1024, file)) != 0) {
        SHA256_Update(&sha256_ctx, data, bytes);
    }
    SHA256_Final(hash, &sha256_ctx);
}

void triplet_print(struct file_triplet *triplet) {
    printf(" FILENAME: %s", triplet->filename);
    printf(" FILESIZE: %lu", triplet->filesize);
    printf(" FILE_HASH: %lu\n", (uint64_t)triplet->hash);
}