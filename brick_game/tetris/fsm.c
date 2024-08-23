#include "fsm.h"

static void start_fn(GameInfoExtended_t *game_info);
static void spawn_fn(GameInfoExtended_t *game_info);
static void moving_fn(GameInfoExtended_t *game_info);
static void pause_fn(GameInfoExtended_t *game_info);
static void shifting_fn(GameInfoExtended_t *game_info);
static void attaching_fn(GameInfoExtended_t *game_info);
static void game_over_fn(GameInfoExtended_t *game_info);
static void exit_fn(GameInfoExtended_t *game_info);

GameInfoExtended_t *current_game_info() {
  static GameInfoExtended_t game_info = {0};
  return &game_info;
}

void finite_state_machine_fn(GameInfoExtended_t *game_info) {
  switch (game_info->state) {
    case START:
      start_fn(game_info);
      break;
    case SPAWN:
      spawn_fn(game_info);
      break;
    case MOVING:
      moving_fn(game_info);
      break;
    case PAUSE:
      pause_fn(game_info);
      break;
    case SHIFTING:
      shifting_fn(game_info);
      break;
    case ATTACHING:
      attaching_fn(game_info);
      break;
    case GAMEOVER:
      game_over_fn(game_info);
      break;
    case EXIT_STATE:
      exit_fn(game_info);
      break;
    default:
      break;
  }
  prepare_game_field(game_info);
}

static void start_fn(GameInfoExtended_t *game_info) {
  if (game_info->new_input) {
    switch (game_info->input) {
      case Start:
        if (game_info->game_inf.pause == 2) {  // после game_over pause = 2
          restart_fn(game_info);
        }
        init_fn(game_info);
        generate_next_fn(game_info);
        game_info->state = SPAWN;
        break;
      case Terminate:
        game_info->state = EXIT_STATE;
        break;
      default:
        break;
    }
  }
  game_info->new_input = false;
}

static void spawn_fn(GameInfoExtended_t *game_info) {
  next_to_figure(game_info);
  generate_next_fn(game_info);
  block_to_move_field(game_info);
  game_info->state = MOVING;
}

static void moving_fn(GameInfoExtended_t *game_info) {
  if (game_info->new_input) switch (game_info->input) {
      case Action:
        action_fn(game_info);
      case Up:
        break;
      case Down:
        game_info->state = SHIFTING;
        break;
      case Right:
        move_block_right(game_info);
        break;
      case Left:
        move_block_left(game_info);
        break;
      case Terminate:
        game_info->state = EXIT_STATE;
        break;
      case Pause:
        game_info->state = PAUSE;
        game_info->game_inf.pause = 1;
        break;
      default:
        break;
    }
  if (check_move_to_shift_fn(game_info)) {
    game_info->state = SHIFTING;
  }
  game_info->new_input = false;
}

static void pause_fn(GameInfoExtended_t *game_info) {
  if (game_info->new_input) {
    switch (game_info->input) {
      case Pause:
        game_info->game_inf.pause = 0;
        game_info->state = MOVING;
        break;
      case Terminate:
        game_info->state = EXIT_STATE;
        break;
      default:
        break;
    }
  }
  game_info->new_input = false;
}

static void shifting_fn(GameInfoExtended_t *game_info) {
  if (check_shifting_to_attaching(game_info)) {
    game_info->state = ATTACHING;
  } else {
    move_block(game_info);
    game_info->state = MOVING;
  }
}

static void attaching_fn(GameInfoExtended_t *game_info) {
  move_from_move_to_touch(game_info);
  count_game_score(game_info);
  if (check_game_over(game_info)) {
    game_info->state = GAMEOVER;
  } else {
    game_info->state = SPAWN;
  }
}

static void game_over_fn(GameInfoExtended_t *game_info) {
  game_info->state = START;
  game_info->game_inf.pause = 2;  // game_over
}

static void exit_fn(GameInfoExtended_t *game_info) {
  dealloc_field(&(game_info->game_inf.field), ROWS_MAP);
  dealloc_field(&(game_info->game_inf.next), NEXT_SIZE);
  game_info->game_inf.pause = 3;  // terminate app
}
