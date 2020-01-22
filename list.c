#include <stdlib.h>
#include "list.h"

static inline SList *snode_calloc(void *data)
{
    SList *ret = NULL;

    ret = calloc(1, sizeof(struct SList));
    if (!ret) {
        return NULL;
    }

    ret->data = data;
    return ret;
}

SList *slist_prepend(SList *list, void *data)
{
    SList *ret = NULL;

    ret = snode_calloc(data);
    if (!ret) {
        return NULL;
    }
    ret->next = list;
    return ret;
}

static inline SList *slist_tail(SList *list)
{
    SList *node = list;

    while (node && node->next) {
        node = node->next;
    }

    return node;
}

SList *slist_append(SList *list, void *data)
{
    SList *tail = NULL;
    SList *node = NULL;

    /* Create new storage link */
    node = snode_calloc(data);
    if (!node) {
        return NULL;
    }

    /* Find the tail if a list exists. */
    tail = slist_tail(list);

    /* We're the new list */
    if (!tail) {
        return node;
    }

    /* Link into the chain */
    tail->next = node;
    return list;
}

bool slist_remove(SList **list, void *data)
{
    SList *head = *list;
    SList *current = head;
    SList *prev = head;

    while ((current != NULL) && (current->data != data)) {
        prev = current;
        current = current->next;
    }

    if (!current) {
        return false;
    }

    /* Data on the head (head removal) */
    if (current == head) {
        head = head->next;
    } else {
        /* Middle or end */
        prev->next = current->next;
    }

    free(current->data);
    free(current);

    *list = head;
    return true;
}

void slist_free(SList *list, free_func freer)
{
    SList *node = list;
    SList *next = NULL;

    while (node) {
        next = node->next;
        if (freer && node->data) {
            freer(node->data);
        }
        free(node);
        node = next;
    }
}

unsigned int slist_length(SList *list)
{
    SList *node = list;
    unsigned int length = 0;

    while (node) {
        node = node->next;
        ++length;
    }

    return length;
}