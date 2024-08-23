#include "tetris_lib.h"

#include "fsm.h"

void userInput(UserAction_t user_input, bool hold) {
  (void) hold;
  GameInfoExtended_t* game = current_game_info();
  if (!game->new_input) {
    game->input = user_input;
    game->new_input = true;
  }
}

GameInfo_t updateCurrentState() {
  GameInfoExtended_t* game = current_game_info();
  finite_state_machine_fn(game);

  return game->game_inf;
}
