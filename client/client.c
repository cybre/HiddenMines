#include <stdio.h>
#include <winsock2.h>
#include <windows.h>
#include <unistd.h>
#include "client.h"
#include "ui.h"

SOCKET client_socket = INVALID_SOCKET;
bool game_is_running = false;

void cleanup()
{
    CloseHandle(communication_thread);
    CloseHandle(chat_thread);
    CloseHandle(game_thread);
    closesocket(client_socket);
    WSACleanup();
}

void send_command(CommandType type, char *parameter)
{
    if (parameter) chomp(parameter);
    char *message = malloc(DEFAULT_BUFLEN);
    sprintf(message, "%d%s", (int)type, parameter);
    send(client_socket, message, DEFAULT_BUFLEN, 0);
}

void send_guess(int row, int column)
{
    Field field;
    field.row = row;
    field.column = column;
    field.distance = 0;
    send_command(PLAYER_GUESS, create_field_message(field));
}

void clear_screen()
{
    system("cls");
}

int main(int argc, char *argv[])
{
    clear_screen();

    WSADATA wsa;

    printf("\nInitialising Winsock...\n");
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Could not initialize Winsock: %d", WSAGetLastError());
        return 1;
    }
    printf("Winsock initialised.\n");

    printf("\nCreating socket...\n");
    client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (client_socket == INVALID_SOCKET) {
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

    struct sockaddr_in target;
    target.sin_family = AF_INET;
    target.sin_port = htons(port);
    target.sin_addr.s_addr = inet_addr(ip);

    printf("\nConnecting to %s:%d...\n", ip, port);
    if (connect(client_socket, (SOCKADDR *)&target, sizeof(target)) == SOCKET_ERROR)
    {
        int result = WSAGetLastError();
        if (result == WSAECONNREFUSED) {
            printf("Connection refused.");
        } else {
            printf("Could not connect to server: %d", result);
        }
        WSACleanup();
        return 1;
    }
    printf("Successfully connected to server.\n");

    char name[15];
    printf("Enter your name: ");
    fgets(name, 15, stdin);
    send_command(SET_PLAYER_NAME, name);

    set_ui_name(name);
    clear_screen();

    start_communication_loop();
    start_chat_loop();

    WaitForSingleObject(chat_thread, INFINITE);
    sleep(1);

    cleanup();
    return 0;
}