#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include "server.h"
#include "../util.h"

SOCKET listen_socket = INVALID_SOCKET;
SList *list = NULL;

void free_player(void *data)
{
    Player *player = (Player *)data;
    closesocket(player->socket);
    CloseHandle(player->thread);
    free(data);
}

void send_command_to_player(Player *player, CommandType type, char *parameter)
{
    char *message = malloc(DEFAULT_BUFLEN);
    sprintf(message, "%d%s", (int)type, parameter);
    send(player->socket, message, DEFAULT_BUFLEN, 0);
    free(message);
}

void send_command_to_all_players(CommandType type, char *parameter)
{
    SList *curr = list;
    while (curr) {
        send_command_to_player((Player *)curr->data, type, parameter);
        curr = curr->next;
    }
}

void end_game()
{
    printf("Game ended...\n");
    reset_player_states();
    game_is_running = false;
}

void reset_player_states()
{
    SList *curr = list;
    while (curr) {
        Player *player = (Player *)curr->data;
        player->ready = false;
        player->num_of_hits = 0;
        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 10; j++) {
                player->distance_matrix[i][j] = 0;
            }
        }
        curr = curr->next;
    }
}

unsigned int get_number_of_ready_players()
{
    SList *curr = list;
    int count = 0;

    while (curr) {
        Player *player = (Player *)curr->data;

        if (player->ready) {
            count++;
        }

        curr = curr->next;
    }

    return count;
}

void update_distance_matrices()
{
    int i, j, k, l;

    SList *curr = list;
    while (curr) {
        Player *player = (Player *)curr->data;

        for (i = 0; i < 10; i++) {
            for (j = 0; j < 10; j++) {
                player->distance_matrix[i][j] = INT_MAX;
            }
        }

        for (i = 0; i < 10; i++) {
            for (j = 0; j < 10; j++) {
                // Traversing the whole matrix
                // to find the minimum distance.
                for (k = 0; k < 10; k++) {
                    for (l = 0; l < 10; l++) {
                        // If the cell contains a ship, check
                        // for minimum distance.
                        if (player->matrix[k][l] == FIELD_STATUS_SHIP) {
                            player->distance_matrix[i][j] = min(player->distance_matrix[i][j],
                                                                max(abs(i - k), abs(j - l)));
                        }
                    }
                }
            }
        }

        curr = curr->next;
    }
}

unsigned int find_nearest_ship(Player *me, Field field)
{
    SList *curr = list;

    int min = 500;

    while (curr) {
        Player *player = (Player *)curr->data;

        if (player != me) {
            int distance = player->distance_matrix[field.row][field.column];
            if (distance < min) min = distance;
        }

        curr = curr->next;
    }

    return min;
}

void check_opponents_for_hits(Player *me, char *field_message) {
    Field field = parse_matrix_field(field_message);

    bool hit = false;
    SList *curr = list;
    while (curr) {
        Player *player = (Player *)curr->data;
        if (player != me && player->matrix[field.row][field.column] == FIELD_STATUS_SHIP) {
            player->matrix[field.row][field.column] = FIELD_STATUS_HIT;
            update_distance_matrices();
            hit = true;
            send_command_to_player(player, OPPONENT_HIT, field_message);
        }
        curr = curr->next;
    }

    field.distance = find_nearest_ship(me, field);

    if (hit) {
        send_command_to_player(me, PLAYER_HIT, create_field_message(field));
        me->num_of_hits++;

        if (me->num_of_hits == 5) {
            send_command_to_all_players(SHOW_WINNER, me->name);
            end_game();
        }
    } else {
        send_command_to_player(me, PLAYER_MISS, create_field_message(field));
    }
}

int get_random_number(int lower, int upper)
{
    return (rand() % (upper - lower + 1)) + lower;
}

void generate_player_matrices()
{
    int i, j;

    SList *curr = list;
    while (curr) {
        Player *player = (Player *)curr->data;
        for (i = 0; i < 10; i++) {
            for (j = 0; j < 10; j++) {
                player->matrix[i][j] = FIELD_STATUS_UNTOUCHED;
            }
        }
        int ship_count = 0;
        while (ship_count < 5) {
            int rand_row = get_random_number(0, 9);
            int rand_column = get_random_number(0, 9);
            if (player->matrix[rand_row][rand_column] == FIELD_STATUS_UNTOUCHED) {
                player->matrix[rand_row][rand_column] = FIELD_STATUS_SHIP;
                Field field;
                field.row = rand_row;
                field.column = rand_column;
                field.distance = 0;
                send_command_to_player(player, ADD_SHIP, create_field_message(field));
                ship_count++;
            }
        }

        update_distance_matrices();

        curr = curr->next;
    }
}

int main(int argc, char *argv[]) {
    WSADATA wsa;

    printf("\nInitialising Winsock...\n");
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Could not initialise Winsock: %d", WSAGetLastError());
        return 1;
    }
    printf("Winsock initialised.\n");

    printf("\nCreating socket...\n");
    listen_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_socket == INVALID_SOCKET) {
        printf("Could not create socket: %d", WSAGetLastError());
        WSACleanup();
        return 1;
    }
    printf("Socket created.\n");

    char *ip = DEFAULT_IP;
    int port = DEFAULT_PORT;

    if (argc == 3) {
        ip = argv[1];
        port = strtol(argv[2], NULL, 10);
    }

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = inet_addr(ip);

    printf("\nBinding socket to port %d...\n", DEFAULT_PORT);
    if (bind(listen_socket, (SOCKADDR *)&address, sizeof(address)) == SOCKET_ERROR) {
        printf("Could not bind port: %d", WSAGetLastError());
        closesocket(listen_socket);
        WSACleanup();
        return 1;
    }
    printf("Socket bound.\n");

    printf("\nCreating listener...\n");
    if (listen(listen_socket, 2) == SOCKET_ERROR)
    {
        printf("Could not create listener: %d", WSAGetLastError());
        closesocket(listen_socket);
        WSACleanup();
        return 1;
    }
    printf("Listener created.\n");

    srand((unsigned)time(NULL));

    printf("\nAccepting clients...\n");
    start_lobby_loop();

    WaitForSingleObject(lobby_thread, INFINITE);
    CloseHandle(lobby_thread);

    slist_free(list, free_player);

    return 0;
}