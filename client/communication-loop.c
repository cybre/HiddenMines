#include <stdio.h>
#include "client.h"
#include "../common.h"
#include "ui.h"

HANDLE communication_thread;
HANDLE stop_communication_event;

void start_communication_loop()
{
    stop_communication_event = CreateEvent(NULL, TRUE, FALSE, NULL);
    communication_thread = CreateThread(NULL, 0, communication_loop, NULL, 0, NULL);
}

DWORD WINAPI communication_loop()
{
    int result = 0;

    do {
        char *received_buf = malloc(DEFAULT_BUFLEN);
        result = recv(client_socket, received_buf, DEFAULT_BUFLEN, 0);
        if (result <= 0) {
            printf("recv failed: %d\n", WSAGetLastError());
            exit(1);
        }

        Command command = parse_message(received_buf);

        switch (command.command_type) {
            case SEND_MESSAGE: {
                printf("%s\n", command.parameter);
                break;
            }
            case START_GAME: {
                reset_matrices();
                start_game_loop();
                break;
            }
            case SHOW_WINNER: {
                display_winner(command.parameter);
                Sleep(3000);
                stop_game_interface();
                break;
            }
            case END_GAME: {
                Sleep(500);
                if (game_is_running) {
                    stop_game_interface();
                }
                break;
            }
            case PLAYER_HIT: {
                Field field = parse_matrix_field(command.parameter);
                update_their_matrix(field, FIELD_STATUS_HIT);
                break;
            }
            case PLAYER_MISS: {
                Field field = parse_matrix_field(command.parameter);
                update_their_matrix(field, FIELD_STATUS_MISS);
                pause_turn();
                break;
            }
            case OPPONENT_HIT: {
                Field field = parse_matrix_field(command.parameter);
                update_my_matrix(field, FIELD_STATUS_HIT);
                break;
            }
            case ADD_SHIP: {
                Field field = parse_matrix_field(command.parameter);
                update_my_matrix(field, FIELD_STATUS_SHIP);
                break;
            }
            default: {
                printf("Unknown command type: %d\n", (int)command.command_type);
            }
        }

        free(received_buf);
        free(command.parameter);
    } while (result > 0 && WaitForSingleObject(stop_communication_event, 1) == WAIT_TIMEOUT);
}