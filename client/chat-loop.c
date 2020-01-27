#include <stdio.h>
#include "client.h"

HANDLE chat_thread;

void start_chat_loop()
{
    chat_thread = CreateThread(NULL, 0, chat_loop, NULL, 0, NULL);
}

DWORD WINAPI chat_loop()
{
    while (WaitForSingleObject(stop_client_event, 1) == WAIT_TIMEOUT) {
        char *messageBuffer = malloc(DEFAULT_BUFLEN - 2);
        fgets(messageBuffer, DEFAULT_BUFLEN - 2, stdin);
        chomp(messageBuffer);
        if (strcmp(messageBuffer, "/leave") == 0) {
            free(messageBuffer);
            printf("Zatvaram konekciju...");
            SetEvent(stop_client_event);
            break;
        }
        if (strcmp(messageBuffer, "/ready") == 0) {
            send_command(START_GAME, NULL);
            continue;
        }
        send_command(SEND_MESSAGE, messageBuffer);
        free(messageBuffer);
    }

    return 0;
}