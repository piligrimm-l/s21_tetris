#ifndef FSM_H
#define FSM_H

#include "tetris_backend.h"

GameInfoExtended_t *current_game_info();
void finite_state_machine_fn(GameInfoExtended_t *game_info);

#endif
