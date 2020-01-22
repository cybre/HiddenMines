#pragma once

#define DEFAULT_PORT 5001
#define DEFAULT_IP "127.0.0.1"
#define DEFAULT_BUFLEN 512

typedef enum {
    FIELD_STATUS_UNTOUCHED,
    FIELD_STATUS_MISS,
    FIELD_STATUS_HIT,
    FIELD_STATUS_SHIP,
} FieldStatus;

typedef enum {
    SET_PLAYER_NAME = 10,
    SEND_MESSAGE,
    START_GAME,
    END_GAME,
    GAME_CLIENT_END,
    PLAYER_GUESS,
    PLAYER_HIT,
    PLAYER_MISS,
    OPPONENT_HIT,
    ADD_SHIP,
    SHOW_WINNER,
} CommandType;

typedef struct {
    CommandType command_type;
    char *parameter;
} Command;

Command parse_message(char *message);

typedef struct {
    int row;
    int column;
    int distance;
} Field;

char *create_field_message(Field field);
Field parse_matrix_field(const char *message);