//
// Created by behruz on 09.06.2021.
//

#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "read_file.h"
#include "file_triplet.h"

static uint8_t is_special_dir(char *dir_name) {
    return (strcmp(dir_name, ".") == 0 || strcmp(dir_name, "..") == 0);
}

static uint8_t is_dir(struct dirent *entry) {
    return 4 == entry->d_type;
}

static void print_space(uint32_t space_count) {
    for (int i = 0; i < space_count; i++) {
        printf(" ");
    }
}

static void get_hex_chars(uint8_t byte, char *hex) {
    unsigned char firstNibble;  // a Nibble is 4 bits, half a byte, one hexadecimal character
    unsigned char secondNibble;


    firstNibble = (byte >> 4);  // isolate first 4 bits

    if (firstNibble < 10U) {
        hex[0] = (char) ('0' + firstNibble);
    } else {
        firstNibble -= 10U;
        hex[0] = (char) ('A' + firstNibble);
    }

    secondNibble = (byte & 0x0F);  // isolate last 4 bits

    if (secondNibble < 10U) {
        hex[1] = (char) ('0' + secondNibble);
    } else {
        secondNibble -= 10U;
        hex[1] = (char) ('A' + secondNibble);
    }
}


void list_add_front_file_triplet(struct list **l, const char *dir_path, const char *file_name,
                                 const char *filename_with_path) {
    char filename[512] = {0};
    char relative_path[512] = {0};
    strcpy(filename, dir_path);
    strcat(filename, "/");
    strcat(filename, file_name);

    if (filename_with_path != NULL) {
        strcat(relative_path, filename_with_path);
    }
    strcat(relative_path, file_name);

    struct file_triplet *triplet = (struct file_triplet *) malloc(sizeof(struct file_triplet));
    FILE *file = fopen(filename, "rb");
//    if (file == NULL) {
//        perror("Can't open file ");
//        return;
//    }
    triplet->filename = malloc(256);
    triplet->filepath = malloc(512);
    strcpy(triplet->filename, filename);
    strcpy(triplet->filepath, relative_path);

    fseek(file, 0, SEEK_END);
    triplet->filesize = ftell(file);
    fseek(file, 0, SEEK_SET);

    uint8_t hash[SHA256_DIGEST_LENGTH] = {0};
    calc_hash(file, hash);

    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        char hex[2];
        get_hex_chars(hash[i], hex);
        triplet->hash[2 * i] = hex[0];
        triplet->hash[2 * i + 1] = hex[1];
    }

    fclose(file);
    list_add_front(l, triplet);

}

int read_directory(char *dir_path, uint32_t space_count, struct list **l, char *relative_path) {
    errno = 0;
    DIR *directory = opendir(dir_path);

    if (directory == NULL) {
        //perror("Can't read directory ");
        return -1;
    }

    uint32_t current_space_count = space_count + 2;

    struct dirent *dir_entry;
    while ((dir_entry = readdir(directory))) {
        if (is_special_dir(dir_entry->d_name)) {
            continue;
        }
        //print_space(current_space_count);

        if (is_dir(dir_entry)) {
            //printf("|-- DIR: %s\n", dir_entry->d_name);
            char full_path[512] = {0};
            strcpy(full_path, dir_path);
            strcat(full_path, "/");
            strcat(full_path, dir_entry->d_name);

            char full_relative_path[512] = {0};
            if (relative_path != NULL) {
                strcat(full_relative_path, relative_path);
            }
            strcat(full_relative_path, dir_entry->d_name);
            strcat(full_relative_path, "/");

            read_directory(full_path, current_space_count, l, full_relative_path);
        } else {
            //printf("|-- FILE: %s\n", dir_entry->d_name);
            list_add_front_file_triplet(l, dir_path, dir_entry->d_name, relative_path);
        }
    }
    closedir(directory);
    return 0;
}

