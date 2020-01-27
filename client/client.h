#pragma once

#include <stdbool.h>
#include <winsock2.h>
#include <windows.h>
#include "../common.h"
#include "../util.h"

extern SOCKET client_socket;

extern HANDLE stop_client_event;

extern HANDLE chat_thread;
extern HANDLE communication_thread;
extern HANDLE game_thread;

extern bool game_is_running;

void cleanup();
void send_command(CommandType type, char *parameter);

void start_communication_loop();
DWORD WINAPI communication_loop();

void start_chat_loop();
DWORD WINAPI chat_loop();

void start_game_loop();
DWORD WINAPI game_loop();