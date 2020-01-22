#include "../player.h"
#include "server.h"
#include "../list.h"
#include "../common.h"

bool game_is_running = false;

void start_player_loop(Player *player)
{
    player->thread = CreateThread(NULL, 0, player_loop, (void *)player, 0, NULL);
}

DWORD WINAPI player_loop(void *data) {
    Player *player = (Player *)data;
    int result = 0;

    printf("\nListening for messages from client.\n");
    do {
        char *received_buf = malloc(DEFAULT_BUFLEN);
        result = recv(player->socket, received_buf, DEFAULT_BUFLEN, 0);

        if (result <= 0) {
            printf("recv failed: %d\n", WSAGetLastError());
            break;
        }

        Command command = parse_message(received_buf);

        char *message = malloc(DEFAULT_BUFLEN - 2);
        switch (command.command_type) {
            case SET_PLAYER_NAME:
                strcpy(player->name, command.parameter);
                sprintf(message, "%s se pridruzio igri!", player->name);
                send_command_to_all_players(SEND_MESSAGE, message);
                break;
            case SEND_MESSAGE: {
                sprintf(message, "%s: %s", player->name, command.parameter);
                send_command_to_all_players(SEND_MESSAGE, message);
                break;
            }
            case START_GAME: {
                if (game_is_running) {
                    send_command_to_player(player, SEND_MESSAGE, "Igra je vec u toku");
                    break;
                }

                if (slist_length(list) < MIN_CLIENTS) {
                    send_command_to_player(player, SEND_MESSAGE, "Nedovoljan broj igraca!");
                    break;
                }

                player->ready = true;

                unsigned int num_ready = get_number_of_ready_players();
                if (num_ready == slist_length(list)) {
                    printf("\nStarting game...\n");
                    send_command_to_all_players(SEND_MESSAGE, "Svi igraci su spremni. Zapocinjemo igru!");
                    send_command_to_all_players(START_GAME, NULL);
                    generate_player_matrices();
                    game_is_running = true;
                } else {
                    sprintf(message, "%d/%d igraca spremno!", num_ready, slist_length(list));
                    send_command_to_all_players(SEND_MESSAGE, message);
                }

                break;
            }
            case PLAYER_GUESS: {
                check_opponents_for_hits(player, command.parameter);
                break;
            }
            case GAME_CLIENT_END: {
                player->ready = false;
                unsigned int num_ready = get_number_of_ready_players();
                if (num_ready < MIN_CLIENTS) {
                    send_command_to_all_players(END_GAME, NULL);
                    end_game();
                }
                break;
            }
            default:
                printf("Unknown message type: %d\n", (int)command.command_type);
        }
        free(received_buf);
        free(message);
        free(command.parameter);
    } while(result > 0);

    if (strlen(player->name) > 0) {
        printf("%s left the game.\n", player->name);
        char *leaveMessage = malloc(DEFAULT_BUFLEN - 2);
        sprintf(leaveMessage, "%s je napustio igru!", player->name);
        send_command_to_all_players(SEND_MESSAGE, leaveMessage);
        free(leaveMessage);
    }

    closesocket(player->socket);
    CloseHandle(player->thread);
    slist_remove(&list, player);

    return 0;
}
