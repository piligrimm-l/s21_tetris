#ifndef C_CLI_H
#define C_CLI_H

#include <curses.h>
#include "../../brick_game/tetris/tetris_lib.h"

void enable_ncurses();
void disable_ncurses();
void frontend_general_fn(GameInfo_t game);

#endif
