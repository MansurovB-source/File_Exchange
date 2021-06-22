//
// Created by behruz on 09.06.2021.
//

#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdint.h>
#include "list.h"
#include "read_file.h"
#include "util.h"

static uint8_t is_special_dir(char *dir_name) {
    return 0 == strcmp(".", dir_name) || 0 == strcmp("..", dir_name);
}

static inline uint8_t is_file(struct dirent *entry) {
    return 8 == entry->d_type;
}

static inline uint8_t is_subdir(struct dirent *entry) {
    return 4 == entry->d_type && !is_special_dir(entry->d_name);
}

void destroy_file_triplet(struct file_triplet *triplet) {
    free(triplet->filename);
    free(triplet->filepath);
    free(triplet);
}

void calc_hash(FILE *file, uint8_t *hash) {
    SHA256_CTX sha256_context;
    unsigned char data[1024];
    SHA256_Init(&sha256_context);
    size_t bytes;
    while ((bytes = fread(data, 1, 1024, file)) != 0)
        SHA256_Update(&sha256_context, data, bytes);
    SHA256_Final(hash, &sha256_context);
}

struct list *populate_list(struct list *l, const char *dir_path, const char *file_name, const char *name_with_path) {
    char filename[512] = {0};
    char relative_path[512] = {0};
    strcpy(filename, dir_path);
    strcat(filename, "/");
    strcat(filename, file_name);

    strcat(relative_path, name_with_path);
    strcat(relative_path, file_name);

    struct file_triplet *net_triplet = malloc(sizeof(struct file_triplet));

    FILE *file = fopen(filename, "rb");

    net_triplet->filename = malloc(256);
    net_triplet->filepath = malloc(512);
    strcpy(net_triplet->filename, file_name);
    strcpy(net_triplet->filepath, relative_path);

    fseek(file, 0, SEEK_END);
    net_triplet->filesize = ftell(file);
    fseek(file, 0, SEEK_SET);

    uint8_t hash[SHA256_DIGEST_LENGTH] = {0};
    calc_hash(file, hash);

    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        char hex[2];
        get_hex_chars(hash[i], hex);
        net_triplet->hash[2 * i] = hex[0];
        net_triplet->hash[2 * i + 1] = hex[1];
    }

    fclose(file);
    return push(l, net_triplet);
}

void visit_dir(struct list **p_l, char *dir_path, char *dir_name, char *relative_path) {
    char full_relative_path[512] = {0};
    char full_path[512] = {0};
    strcpy(full_path, dir_path);
    strcat(full_path, "/");
    strcat(full_path, dir_name);

    strcat(full_relative_path, relative_path);
    strcat(full_relative_path, dir_name);
    strcat(full_relative_path, "/");

    DIR *dir = opendir(full_path);
    if (dir == 0x0) {
        return;
    }
    struct dirent *entry;
    while ((entry = readdir(dir))) {
        if (is_file(entry)) {
            *p_l = populate_list(*p_l, full_path, entry->d_name, full_relative_path);
        }
        if (is_subdir(entry)) {
            visit_dir(p_l, full_path, entry->d_name, full_relative_path);
        }
    }
    closedir(dir);
}

int8_t run_file_reader(char *dir_path, struct list **p_l) {
    struct list *l = malloc(sizeof(struct list));
    DIR *dir = opendir(dir_path);
    if (dir == 0x0) {
        return -1;
    }
    struct dirent *entry;
    while ((entry = readdir(dir))) {
        if (is_file(entry)) {
            l = populate_list(l, dir_path, entry->d_name, "");
        }
        if (is_subdir(entry)) {
            visit_dir(&l, dir_path, entry->d_name, "");
        }
    }
    closedir(dir);
    *p_l = l;
    return 0;
}