#pragma once

#include <stdio.h>
#include <winsock2.h>
#include <windows.h>
#include "../list.h"
#include "../player.h"
#include "../common.h"

#define MIN_CLIENTS 2

extern SList *list;

extern HANDLE lobby_thread;
extern HANDLE stop_lobby_event;

extern SOCKET listen_socket;

extern bool game_is_running;

void start_player_loop(Player *player);
DWORD WINAPI player_loop(void *data);

void start_lobby_loop();
DWORD WINAPI lobby_loop();

unsigned int get_number_of_ready_players();

void send_command_to_player(Player *player, CommandType type, char *parameter);
void send_command_to_all_players(CommandType type, char *parameter);
void reset_player_states();
void check_opponents_for_hits(Player *me, char *field_message);

void generate_player_matrices();
void end_game();

