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


void list_add_front_file_triplet(struct list **l, const char *dir_path, const char *file_name) {
    char filename[strlen(dir_path) + strlen(file_name) + 2];
    strcpy(filename, dir_path);
    strcat(filename, "/");
    strcat(filename, file_name);

    struct file_triplet *triplet = (struct file_triplet *) malloc(sizeof(struct file_triplet));
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        perror("Can't open file ");
        return;
    }
    triplet->filename = malloc(strlen(filename) + 1);
    strcpy(triplet->filename, filename);
    fseek(file, 0, SEEK_END);
    triplet->filesize = ftell(file);
    fseek(file, 0, SEEK_SET);

    uint8_t hash[SHA256_DIGEST_LENGTH];
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

int read_directory(char *filename, uint32_t space_count, struct list **l) {
    errno = 0;
    DIR *directory = opendir(filename);

    if (directory == NULL) {
        perror("Can't read directory ");
        return -1;
    }

    uint32_t current_space_count = space_count + 2;

    struct dirent *dir_entry;
    while ((dir_entry = readdir(directory))) {
        if (is_special_dir(dir_entry->d_name)) {
            continue;
        }
        print_space(current_space_count);

        if (is_dir(dir_entry)) {
            printf("|-- DIR: %s\n", dir_entry->d_name);
            char dir_name[strlen(filename) + strlen(dir_entry->d_name) + 2];
            strcpy(dir_name, filename);
            strcat(dir_name, "/");
            strcat(dir_name, dir_entry->d_name);
            read_directory(dir_name, current_space_count, l);
        } else {
            printf("|-- FILE: %s\n", dir_entry->d_name);
            list_add_front_file_triplet(l, filename, dir_entry->d_name);
        }
    }
    closedir(directory);
    return 0;
}

