#include "client.h"
#include "ui.h"

HANDLE game_thread;

void start_game_loop()
{
    game_is_running = true;
    game_thread = CreateThread(NULL, 0, game_loop, NULL, 0, NULL);
}

DWORD WINAPI game_loop()
{
    start_ui();
    game_is_running = false;
    send_command(GAME_CLIENT_END, NULL);
    return 0;
}