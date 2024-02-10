#include <stdio.h>
#include <stdlib.h>
#include "../include/list.h"

List *lst_init(size_t item_size) {
    List *lst = malloc(sizeof(List));
    lst->item_size = item_size;
    lst->items = calloc(1, item_size);
    lst->size = 0;
    return lst;
}

void lst_push(List *lst, void *item) {
    lst->items = realloc(lst->items, ++lst->size * lst->item_size);
    lst->items[lst->size - 1] = item;
}
