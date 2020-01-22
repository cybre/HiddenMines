#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "common.h"

Command parse_message(char *message)
{
    Command command;

    char commandType[3];
    strncpy(commandType, message, 2);
    commandType[2] = '\0';

    int messageNumber = strtol(commandType, NULL, 10);
    command.command_type = (CommandType)messageNumber;

    command.parameter = malloc(DEFAULT_BUFLEN - 2);
    strcpy(command.parameter, message + 2);

    return command;
}

char *create_field_message(Field field)
{
    char *field_message = malloc(6);
    sprintf(field_message, "%d-%d-%d", field.row, field.column, field.distance);
    field_message[5] = '\0';
    return field_message;
}

Field parse_matrix_field(const char *message)
{
    Field field;

    field.row = message[0] - '0';
    // message[1] = '-'
    field.column = message[2] - '0';
    // message[3] = '-'
    field.distance = message[4] - '0';

    return field;
}