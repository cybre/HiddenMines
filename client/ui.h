#pragma once

#include "../common.h"

void start_ui();
void send_guess(int row, int column);

void reset_matrices();
void update_their_matrix(Field field, FieldStatus value);
void update_my_matrix(Field field, FieldStatus value);
void pause_turn();
void set_ui_name(char *name);
void display_winner(char *name);
void stop_game_interface();