#ifndef C_TETRIS_BACKEND_H
#define C_TETRIS_BACKEND_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#include "tetris_lib.h"

#define ROWS_MAP 20
#define COLS_MAP 10
#define NEXT_SIZE 4

typedef enum {
  START = 0,
  SPAWN,
  MOVING,
  SHIFTING,
  ATTACHING,
  GAMEOVER,
  PAUSE,
  EXIT_STATE
} State;

typedef struct {
  int block[4][4];
  int x;
  int y;
  char current_type;
  char next_type;
} Figure;

typedef struct {
  GameInfo_t game_inf;
  Figure figure;
  UserAction_t input;
  int move_field[20][10];
  int touched_field[20][10];
  bool new_input;
  State state;
  unsigned long long timer;
  bool timer_activated;
  bool mem_alloc;
  bool high_score_readed;
} GameInfoExtended_t;

void prepare_game_field(GameInfoExtended_t *game_info);
void restart_fn(GameInfoExtended_t *game_info);
void init_fn(GameInfoExtended_t *game_info);
void generate_next_fn(GameInfoExtended_t *game_info);
bool check_game_over(GameInfoExtended_t *game_info);
void next_to_figure(GameInfoExtended_t *game_info);
void block_to_move_field(GameInfoExtended_t *game_info);
void action_fn(GameInfoExtended_t *game_info);
void move_block_right(GameInfoExtended_t *game_info);
void move_block_left(GameInfoExtended_t *game_info);
bool check_move_to_shift_fn(GameInfoExtended_t *game_info);
bool check_shifting_to_attaching(GameInfoExtended_t *game_info);
void move_block(GameInfoExtended_t *game_info);
void move_from_move_to_touch(GameInfoExtended_t *game_info);
void count_game_score(GameInfoExtended_t *game_info);
void dealloc_field(int ***field, int rows);

#endif
