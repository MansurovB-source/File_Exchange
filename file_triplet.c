//
// Created by behruz on 12.06.2021.
//

#include <string.h>
#include "file_triplet.h"

//TODO change hash reading
static struct file_triplet *cmp_triplet(struct file_triplet *t, char *triplet) {
    char triplet_str[2048] = {0};
    strcat(triplet_str, t->filename);
    strcat(triplet_str, " - ");
    char size[512] = {0};
    sprintf(size, "%lu", t->filesize);
    strcat(triplet_str, size);
    strcat(triplet_str, " - ");
    strncat(triplet_str, t->hash, SHA256_DIGEST_LENGTH * 2);
    if (!strcmp(triplet, triplet_str)) {
        return t;
    } else {
        return NULL;
    }
}

void file_triplet_destroy(void *data) {
    struct file_triplet *triplet = data;
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
    printf(" HASH: ");
    for (int i = 0; i < sizeof(triplet->hash); i++) {
        printf("%c", triplet->hash[i]);
    }
    printf("\n");
}

struct file_triplet *find_triplet(struct list *l, char *triplet) {
    struct list *current = l;
    while (current->value != NULL) {
        if (cmp_triplet(current->value, triplet) != NULL) {
            return current->value;
        }
        current = current->next;
    }
    return NULL;
}