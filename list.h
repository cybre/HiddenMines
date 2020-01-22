#pragma once

#include <stdbool.h>

typedef void (*free_func)(void *v);

typedef struct SList {
    void *data;
    struct SList *next;
} SList;

SList *slist_prepend(SList *list, void *data);
SList *slist_append(SList *list, void *data);
bool slist_remove(SList **list, void *data);
void slist_free(SList *list, free_func freer);
unsigned int slist_length(SList *list);