#include "server.h"
#include "../player.h"
#include "../list.h"

HANDLE lobby_thread;
HANDLE stop_lobby_event;

void start_lobby_loop()
{
    stop_lobby_event = CreateEvent(NULL, TRUE, FALSE, NULL);
    lobby_thread = CreateThread(NULL, 0, lobby_loop, NULL, 0, NULL);
}

DWORD WINAPI lobby_loop()
{
    while (WaitForSingleObject(stop_lobby_event, 1) == WAIT_TIMEOUT)
    {
        Player *player = (Player *)malloc(sizeof(Player));
        player->name[0] = '\0';
        player->ready = false;
        player->num_of_hits = 0;
        player->socket = accept(listen_socket, &(player->info), NULL);

        if (player->socket == INVALID_SOCKET)
        {
            continue;
        }

        list = slist_append(list, player);
        printf("Client %d joined.\n", slist_length(list));
        start_player_loop(player);
    }

    return 0;
}