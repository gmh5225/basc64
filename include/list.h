#ifndef LIST_H
#define LIST_H

#include <stdio.h>

typedef struct {
    void **items;
    size_t item_size, size;
} List;

List *lst_init(size_t item_size);

void lst_push(List *lst, void *item);

#endif
