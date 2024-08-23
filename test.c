#include <check.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#include "brick_game/tetris/fsm.h"
#include "brick_game/tetris/tetris_backend.h"
#include "brick_game/tetris/tetris_lib.h"

START_TEST(test1) {
  GameInfoExtended_t game_info = {0};
  restart_fn(&game_info);
  for (int i = 0; i < 20; i++)
    for (int j = 0; j < 10; j++)
      ck_assert_int_eq(0, game_info.move_field[i][j]);
  ck_assert_int_eq(0, game_info.game_inf.score);
  ck_assert_int_eq(0, game_info.game_inf.pause);
  ck_assert_int_eq(1, game_info.game_inf.level);
}
END_TEST

START_TEST(test2) {
  GameInfoExtended_t game_info = {0};
  init_fn(&game_info);
  for (int i = 0; i < 20; i++)
    for (int j = 0; j < 10; j++)
      ck_assert_int_eq(0, game_info.game_inf.field[i][j]);
  for (int i = 0; i < 4; i++)
    for (int j = 0; j < 4; j++)
      ck_assert_int_eq(0, game_info.game_inf.next[i][j]);
  dealloc_field(&(game_info.game_inf.field), 20);
  dealloc_field(&(game_info.game_inf.next), 4);
  ck_assert_ptr_null(game_info.game_inf.field);
  ck_assert_ptr_null(game_info.game_inf.next);
}
END_TEST

START_TEST(test3) {
  GameInfoExtended_t game_info = {0};
  init_fn(&game_info);
  for (int i = 0; i < 19; i++)
    for (int j = 0; j < 10; j++) game_info.move_field[i][j] = 1;
  prepare_game_field(&game_info);
  for (int i = 0; i < 19; i++)
    for (int j = 0; j < 10; j++)
      ck_assert_int_eq(1, game_info.game_inf.field[i][j]);
  for (int i = 0; i < 4; i++)
    for (int j = 0; j < 4; j++)
      ck_assert_int_eq(0, game_info.game_inf.next[i][j]);
  dealloc_field(&(game_info.game_inf.field), 20);
  dealloc_field(&(game_info.game_inf.next), 4);
}
END_TEST

START_TEST(test4) {
  GameInfoExtended_t game_info = {0};
  init_fn(&game_info);
  int i = 0;
  while (i < 1000) {
    generate_next_fn(&game_info);
    i++;
    ck_assert_float_ne(0, game_info.figure.next_type);
  }
  dealloc_field(&(game_info.game_inf.field), 20);
  dealloc_field(&(game_info.game_inf.next), 4);
}
END_TEST

START_TEST(test5) {
  GameInfoExtended_t game_info = {0};
  init_fn(&game_info);
  for (int i = 0; i < 20; i++)
    for (int j = 0; j < 10; j++) game_info.touched_field[i][j] = 1;
  ck_assert_int_eq(1, check_game_over(&game_info));

  dealloc_field(&(game_info.game_inf.field), 20);
  dealloc_field(&(game_info.game_inf.next), 4);
}
END_TEST

START_TEST(test6) {
  GameInfoExtended_t game_info = {0};
  init_fn(&game_info);
  generate_next_fn(&game_info);
  next_to_figure(&game_info);
  ck_assert_int_eq(0, game_info.figure.x);
  ck_assert_int_eq(4, game_info.figure.y);
  ck_assert_int_ne(0, game_info.figure.current_type);
  ck_assert_int_eq(0, game_info.figure.next_type);

  dealloc_field(&(game_info.game_inf.field), 20);
  dealloc_field(&(game_info.game_inf.next), 4);
}

START_TEST(test7) {
  GameInfoExtended_t game_info = {0};
  init_fn(&game_info);
  for (int i = 0; i < 4; i++)
    for (int j = 0; j < 4; j++) game_info.game_inf.next[i][j] = 1;
  next_to_figure(&game_info);
  block_to_move_field(&game_info);
  for (int i = 0; i < 4; i++)
    for (int j = 4; j < 8; j++) ck_assert_int_eq(1, game_info.move_field[i][j]);

  dealloc_field(&(game_info.game_inf.field), 20);
  dealloc_field(&(game_info.game_inf.next), 4);
}

START_TEST(test8) {
  GameInfoExtended_t game_info = {0};
  init_fn(&game_info);
  for (int i = -1; i < 100; i++) {
    generate_next_fn(&game_info);
    next_to_figure(&game_info);
    game_info.figure.y = i % 9;
    action_fn(&game_info);
  }
  dealloc_field(&(game_info.game_inf.field), 20);
  dealloc_field(&(game_info.game_inf.next), 4);
}
START_TEST(test9) {
  GameInfoExtended_t game_info = {0};
  init_fn(&game_info);
  generate_next_fn(&game_info);
  next_to_figure(&game_info);
  move_block_right(&game_info);
  ck_assert_int_eq(5, game_info.figure.y);

  dealloc_field(&(game_info.game_inf.field), 20);
  dealloc_field(&(game_info.game_inf.next), 4);
}
START_TEST(test10) {
  GameInfoExtended_t game_info = {0};
  init_fn(&game_info);
  generate_next_fn(&game_info);
  next_to_figure(&game_info);
  move_block_left(&game_info);
  ck_assert_int_eq(3, game_info.figure.y);

  dealloc_field(&(game_info.game_inf.field), 20);
  dealloc_field(&(game_info.game_inf.next), 4);
}
START_TEST(test11) {
  GameInfoExtended_t game_info = {0};
  game_info.game_inf.level = 1;
  bool res = 0;
  res = check_move_to_shift_fn(&game_info);
  ck_assert_int_eq(0, res);
  ck_assert_int_eq(1, game_info.timer_activated);
  ck_assert_int_ne(0, game_info.timer);
}

START_TEST(test12) {
  GameInfoExtended_t game_info = {0};
  game_info.game_inf.level = 1;
  bool res = 0;
  while (!res) {
    res = check_move_to_shift_fn(&game_info);
  }
  ck_assert_int_eq(1, res);
  ck_assert_int_eq(0, game_info.timer_activated);
  ck_assert_int_eq(0, game_info.timer);
}

START_TEST(test13) {
  GameInfoExtended_t game_info = {0};
  game_info.game_inf.level = 1;
  bool res = 0;
  res = check_shifting_to_attaching(&game_info);
  ck_assert_int_eq(0, res);
  for (int i = 0; i < 20; i++)
    for (int j = 0; j < 10; j++) game_info.touched_field[i][j] = 1;
  for (int i = 0; i < 20; i++)
    for (int j = 0; j < 10; j++) game_info.move_field[i][j] = 1;
  res = check_shifting_to_attaching(&game_info);
  ck_assert_int_eq(1, res);
  for (int i = 0; i < 20; i++)
    for (int j = 0; j < 10; j++) game_info.touched_field[i][j] = 0;
  res = check_shifting_to_attaching(&game_info);
  ck_assert_int_eq(1, res);
}
START_TEST(test14) {
  GameInfoExtended_t game_info = {0};
  init_fn(&game_info);
  generate_next_fn(&game_info);
  next_to_figure(&game_info);
  move_block(&game_info);
  ck_assert_int_eq(1, game_info.figure.x);

  dealloc_field(&(game_info.game_inf.field), 20);
  dealloc_field(&(game_info.game_inf.next), 4);
}
START_TEST(test15) {
  GameInfoExtended_t game_info = {0};
  for (int i = 0; i < 2; i++)
    for (int j = 0; j < 10; j++) game_info.move_field[i][j] = 1;
  move_from_move_to_touch(&game_info);

  for (int i = 0; i < 2; i++)
    for (int j = 0; j < 10; j++)
      ck_assert_int_eq(0, game_info.move_field[i][j]);

  for (int i = 0; i < 2; i++)
    for (int j = 0; j < 10; j++)
      ck_assert_int_eq(1, game_info.touched_field[i][j]);
}
START_TEST(test16) {
  GameInfoExtended_t game_info = {0};
  game_info.game_inf.level = 1;
  for (int i = 19; i < 20; i++)
    for (int j = 0; j < 10; j++) game_info.touched_field[i][j] = 1;
  count_game_score(&game_info);
  ck_assert_int_eq(100, game_info.game_inf.score);
}
START_TEST(test17) {
  GameInfoExtended_t game_info = {0};
  game_info.state = START;
  game_info.new_input = true;
  game_info.input = Start;
  finite_state_machine_fn(&game_info);
  ck_assert_ptr_nonnull(game_info.game_inf.field);
  ck_assert_ptr_nonnull(game_info.game_inf.next);
  ck_assert_int_eq(SPAWN, game_info.state);
  game_info.state = START;
  game_info.new_input = true;
  game_info.input = Terminate;
  finite_state_machine_fn(&game_info);
  ck_assert_int_eq(EXIT_STATE, game_info.state);
  finite_state_machine_fn(&game_info);
  ck_assert_int_eq(3, game_info.game_inf.pause);
  ck_assert_ptr_null(game_info.game_inf.field);
  ck_assert_ptr_null(game_info.game_inf.next);
}

START_TEST(test18) {
  GameInfoExtended_t game_info = {0};
  game_info.state = START;
  game_info.new_input = true;
  game_info.input = Start;
  finite_state_machine_fn(&game_info);
  finite_state_machine_fn(&game_info);
  ck_assert_int_eq(MOVING, game_info.state);
  ck_assert_int_eq(4, game_info.figure.y);
  ck_assert_int_eq(0, game_info.figure.x);

  dealloc_field(&(game_info.game_inf.field), 20);
  dealloc_field(&(game_info.game_inf.next), 4);
}

START_TEST(test19) {
  GameInfoExtended_t game_info = {0};
  game_info.state = START;
  game_info.new_input = true;
  game_info.input = Start;
  finite_state_machine_fn(&game_info);
  finite_state_machine_fn(&game_info);
  game_info.new_input = true;
  game_info.input = Action;
  finite_state_machine_fn(&game_info);
  ck_assert_int_eq(MOVING, game_info.state);

  dealloc_field(&(game_info.game_inf.field), 20);
  dealloc_field(&(game_info.game_inf.next), 4);
}
START_TEST(test20) {
  GameInfoExtended_t game_info = {0};
  game_info.state = START;
  game_info.new_input = true;
  game_info.input = Start;
  finite_state_machine_fn(&game_info);
  finite_state_machine_fn(&game_info);
  game_info.new_input = true;
  game_info.input = Down;
  finite_state_machine_fn(&game_info);
  ck_assert_int_eq(SHIFTING, game_info.state);

  dealloc_field(&(game_info.game_inf.field), 20);
  dealloc_field(&(game_info.game_inf.next), 4);
}
START_TEST(test21) {
  GameInfoExtended_t game_info = {0};
  game_info.state = START;
  game_info.new_input = true;
  game_info.input = Start;
  finite_state_machine_fn(&game_info);
  finite_state_machine_fn(&game_info);
  game_info.new_input = true;
  game_info.input = Right;
  finite_state_machine_fn(&game_info);
  ck_assert_int_eq(MOVING, game_info.state);

  dealloc_field(&(game_info.game_inf.field), 20);
  dealloc_field(&(game_info.game_inf.next), 4);
}
START_TEST(test22) {
  GameInfoExtended_t game_info = {0};
  game_info.state = START;
  game_info.new_input = true;
  game_info.input = Start;
  finite_state_machine_fn(&game_info);
  finite_state_machine_fn(&game_info);
  game_info.new_input = true;
  game_info.input = Left;
  finite_state_machine_fn(&game_info);
  ck_assert_int_eq(MOVING, game_info.state);

  dealloc_field(&(game_info.game_inf.field), 20);
  dealloc_field(&(game_info.game_inf.next), 4);
}
START_TEST(test23) {
  GameInfoExtended_t game_info = {0};
  game_info.state = START;
  game_info.new_input = true;
  game_info.input = Start;
  finite_state_machine_fn(&game_info);
  finite_state_machine_fn(&game_info);
  game_info.new_input = true;
  game_info.input = Terminate;
  finite_state_machine_fn(&game_info);
  ck_assert_int_eq(EXIT_STATE, game_info.state);

  dealloc_field(&(game_info.game_inf.field), 20);
  dealloc_field(&(game_info.game_inf.next), 4);
}
START_TEST(test24) {
  GameInfoExtended_t game_info = {0};
  game_info.state = START;
  game_info.new_input = true;
  game_info.input = Start;
  finite_state_machine_fn(&game_info);
  finite_state_machine_fn(&game_info);
  game_info.new_input = true;
  game_info.input = Pause;
  finite_state_machine_fn(&game_info);
  ck_assert_int_eq(PAUSE, game_info.state);
  ck_assert_int_eq(1, game_info.game_inf.pause);

  dealloc_field(&(game_info.game_inf.field), 20);
  dealloc_field(&(game_info.game_inf.next), 4);
}
START_TEST(test25) {
  GameInfoExtended_t game_info = {0};
  game_info.state = PAUSE;
  game_info.new_input = true;
  game_info.input = Pause;
  finite_state_machine_fn(&game_info);
  ck_assert_int_eq(MOVING, game_info.state);
  ck_assert_int_eq(0, game_info.game_inf.pause);
  game_info.game_inf.pause = 1;
  game_info.state = PAUSE;
  game_info.new_input = true;
  game_info.input = Terminate;
  finite_state_machine_fn(&game_info);
  ck_assert_int_eq(EXIT_STATE, game_info.state);
  ck_assert_int_eq(1, game_info.game_inf.pause);
}
START_TEST(test26) {
  GameInfoExtended_t game_info = {0};
  game_info.state = SHIFTING;
  finite_state_machine_fn(&game_info);
  ck_assert_int_eq(MOVING, game_info.state);
  game_info.state = SHIFTING;
  for (int i = 19; i < 20; i++)
    for (int j = 0; j < 10; j++) game_info.move_field[i][j] = 1;
  finite_state_machine_fn(&game_info);
  ck_assert_int_eq(ATTACHING, game_info.state);
}
START_TEST(test27) {
  GameInfoExtended_t game_info = {0};
  game_info.state = ATTACHING;
  finite_state_machine_fn(&game_info);
  ck_assert_int_eq(SPAWN, game_info.state);
}
START_TEST(test28) {
  GameInfoExtended_t game_info = {0};
  game_info.state = GAMEOVER;
  finite_state_machine_fn(&game_info);
  ck_assert_int_eq(START, game_info.state);
  ck_assert_int_eq(2, game_info.game_inf.pause);
}

START_TEST(test29) {
  GameInfoExtended_t* game_info = NULL;
  game_info = current_game_info();
  ck_assert_ptr_nonnull(game_info);
}

START_TEST(test30) {
  GameInfoExtended_t* game_info = NULL;
  game_info = current_game_info();
  GameInfo_t game_inf = {0};
  game_inf = updateCurrentState();
  ck_assert_ptr_null(game_inf.field);
  ck_assert_ptr_null(game_inf.next);
  game_info->new_input = true;
  game_info->input = Start;
  game_inf = updateCurrentState();
  ck_assert_ptr_nonnull(game_inf.field);
  ck_assert_ptr_nonnull(game_inf.next);

  dealloc_field(&(game_info->game_inf.field), 20);
  dealloc_field(&(game_info->game_inf.next), 4);
}
START_TEST(test31) {
  GameInfoExtended_t* game_info = NULL;
  game_info = current_game_info();
  userInput(Action, false);
  ck_assert_int_eq(Action, game_info->input);
  ck_assert_int_eq(1, game_info->new_input);
}
int main() {
  Suite* suite = suite_create("UNITS");
  TCase* tcase = tcase_create("TESTS");
  SRunner* srunner = srunner_create(suite);
  int val = 0;

  suite_add_tcase(suite, tcase);

  tcase_add_test(tcase, test1);
  tcase_add_test(tcase, test2);
  tcase_add_test(tcase, test3);
  tcase_add_test(tcase, test4);
  tcase_add_test(tcase, test5);
  tcase_add_test(tcase, test6);
  tcase_add_test(tcase, test7);
  tcase_add_test(tcase, test8);
  tcase_add_test(tcase, test9);
  tcase_add_test(tcase, test10);
  tcase_add_test(tcase, test11);
  tcase_add_test(tcase, test12);
  tcase_add_test(tcase, test13);
  tcase_add_test(tcase, test14);
  tcase_add_test(tcase, test15);
  tcase_add_test(tcase, test16);
  tcase_add_test(tcase, test17);
  tcase_add_test(tcase, test18);
  tcase_add_test(tcase, test19);
  tcase_add_test(tcase, test20);
  tcase_add_test(tcase, test21);
  tcase_add_test(tcase, test22);
  tcase_add_test(tcase, test23);
  tcase_add_test(tcase, test24);
  tcase_add_test(tcase, test25);
  tcase_add_test(tcase, test26);
  tcase_add_test(tcase, test27);
  tcase_add_test(tcase, test28);
  tcase_add_test(tcase, test29);
  tcase_add_test(tcase, test30);
  tcase_add_test(tcase, test31);

  srunner_run_all(srunner, CK_NORMAL);
  val = srunner_ntests_failed(srunner);
  srunner_free(srunner);
  return val == 0 ? 0 : 1;
}
