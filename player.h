#pragma once

#include <stdbool.h>
#include <winsock2.h>
#include <windows.h>

typedef struct {
    HANDLE thread;
    SOCKET socket;
    bool ready;
    struct sockaddr info;
    char name[16];
    int matrix[10][10];
    int distance_matrix[10][10];
    int num_of_hits;
} Player;

void free_player(void *data);