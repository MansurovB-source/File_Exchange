//
// Created by behruz on 09.06.2021.
//

#ifndef FILE_EXCHANGE_READ_FILE_H
#define FILE_EXCHANGE_READ_FILE_H

#include <stdint.h>
#include "list.h"

int read_directory(char *filename, uint32_t space_count, struct list **l);


#endif //FILE_EXCHANGE_READ_FILE_H