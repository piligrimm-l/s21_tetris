#include "tetris_backend.h"

static void fill_zero_fn(int **field, int rows, int cols);
static void alloc_field_fn(int ***field, int rows, int cols);
static unsigned long long current_time_millis();
static void reset_static_field(int move_field[][COLS_MAP]);
static void fill_next_fn(GameInfo_t *game_inf, int next[][4]);
static bool check_collision_bottom(int move_field[][COLS_MAP]);
static bool check_collision_left(GameInfoExtended_t *game_info);
static bool check_collision_right(GameInfoExtended_t *game_info);
static bool check_collision_figure(int move_field[][COLS_MAP],
                                      int touched_field[][COLS_MAP]);
static void delete_filled_line(int touched_field[][COLS_MAP], int line);
static int check_filled_line(int touched_field[][COLS_MAP]);
static void cp_tetr_to_tetr(Figure *dst, Figure *src);
static void turn_stick_fn(Figure *figure);
static void turn_other_fn(Figure *figure);
static bool check_tetr_collision_after_action(GameInfoExtended_t *game_info);
static void check_y_after_action(GameInfoExtended_t *game_info);
static bool check_x_after_action(GameInfoExtended_t *game_info);
static void set_lvl_fn(GameInfoExtended_t *game_info);
static void save_max_score(GameInfoExtended_t *game_info);
static void load_max_score(GameInfoExtended_t *game_info);
static unsigned long long time_diff_fn(unsigned long long timer);

void prepare_game_field(GameInfoExtended_t *game_info) {
  if (game_info->game_inf.field != NULL) {
    for (int i = 0; i != ROWS_MAP; ++i) {
      for (int j = 0; j != COLS_MAP; ++j) {
        if (game_info->move_field[i][j] == 1 ||
            game_info->touched_field[i][j] == 1) {
          game_info->game_inf.field[i][j] = 1;
        } else {
          game_info->game_inf.field[i][j] = 0;
        }
      }
    }
  }
}

void restart_fn(GameInfoExtended_t *game_info) {
  reset_static_field(game_info->move_field);
  reset_static_field(game_info->touched_field);
  game_info->game_inf.score = 0;
  game_info->game_inf.pause = 0;
  game_info->game_inf.level = 1;
}

void init_fn(GameInfoExtended_t *game_info) {
  if (!game_info->mem_alloc) {
    alloc_field_fn(&(game_info->game_inf.field), ROWS_MAP, COLS_MAP);
    alloc_field_fn(&(game_info->game_inf.next), NEXT_SIZE, NEXT_SIZE);
    game_info->mem_alloc = true;
  }
  game_info->game_inf.level = 1;
  load_max_score(game_info);
}

void generate_next_fn(GameInfoExtended_t *game_info) {
  int index = rand() % 7;
  if (index == 0) {
    int i[4][4] = {{0, 0, 0, 0}, {1, 1, 1, 1}, {0}, {0}};
    fill_next_fn(&(game_info->game_inf), i);
    game_info->figure.next_type = 'i';
  }
  if (index == 1) {
    int o[4][4] = {{0, 1, 1, 0}, {0, 1, 1, 0}, {0}, {0}};
    fill_next_fn(&(game_info->game_inf), o);
    game_info->figure.next_type = 'o';
  }
  if (index == 2) {
    int t[4][4] = {{0, 1, 0, 0}, {1, 1, 1, 0}, {0}, {0}};
    fill_next_fn(&(game_info->game_inf), t);
    game_info->figure.next_type = 't';
  }
  if (index == 3) {
    int j[4][4] = {{1, 0, 0, 0}, {1, 1, 1, 0}, {0}, {0}};
    fill_next_fn(&(game_info->game_inf), j);
    game_info->figure.next_type = 'j';
  }
  if (index == 4) {
    int l[4][4] = {{0, 0, 1, 0}, {1, 1, 1, 0}, {0}, {0}};
    fill_next_fn(&(game_info->game_inf), l);
    game_info->figure.next_type = 'l';
  }
  if (index == 5) {
    int s[4][4] = {{0, 1, 1, 0}, {1, 1, 0, 0}, {0}, {0}};
    fill_next_fn(&(game_info->game_inf), s);
    game_info->figure.next_type = 's';
  }
  if (index == 6) {
    int z[4][4] = {{1, 1, 0, 0}, {0, 1, 1, 0}, {0}, {0}};
    fill_next_fn(&(game_info->game_inf), z);
    game_info->figure.next_type = 'z';
  }
}

bool check_game_over(GameInfoExtended_t *game_info) {
  bool res = false;
  for (int j = 0; j != COLS_MAP; ++j) {
    if (game_info->touched_field[0][j] == 1) {
      res = true;
    }
  }
  return res;
}

void next_to_figure(GameInfoExtended_t *game_info) {
  for (int i = 0; i != 4; ++i) {
    for (int j = 0; j != 4; ++j) {
      game_info->figure.block[i][j] = game_info->game_inf.next[i][j];
    }
  }
  game_info->figure.x = 0;  // высота поялвения блока
  game_info->figure.y = 4;  // ширина появления блока
  game_info->figure.current_type = game_info->figure.next_type;
  game_info->figure.next_type = 0;
}

void block_to_move_field(GameInfoExtended_t *game_info) {
  int x = game_info->figure.x;
  int y = game_info->figure.y;
  reset_static_field(game_info->move_field);
  for (int i = 0; i != 4; ++i) {
    for (int j = 0; j != 4; ++j) {
      if (game_info->figure.block[i][j] == 1) {
        game_info->move_field[i + x][j + y] = 1;
      }
    }
  }
}

void action_fn(GameInfoExtended_t *game_info) {
  Figure tmp_tetr = {0};
  cp_tetr_to_tetr(&tmp_tetr, &(game_info->figure));
  if (game_info->figure.current_type == 'o') {
    ;
  } else if (game_info->figure.current_type == 'i') {
    turn_stick_fn(&(game_info->figure));
  } else {
    turn_other_fn(&(game_info->figure));
  }
  if (check_x_after_action(game_info)) {
    cp_tetr_to_tetr(&(game_info->figure), &tmp_tetr);
  }
  check_y_after_action(game_info);
  block_to_move_field(game_info);
  if (check_tetr_collision_after_action(game_info)) {
    cp_tetr_to_tetr(&(game_info->figure), &tmp_tetr);
    block_to_move_field(game_info);
  }
}

void move_block_right(GameInfoExtended_t *game_info) {
  if (!check_collision_right(game_info)) {
    ++game_info->figure.y;
    block_to_move_field(game_info);
  }
}

void move_block_left(GameInfoExtended_t *game_info) {
  if (!check_collision_left(game_info)) {
    --game_info->figure.y;
    block_to_move_field(game_info);
  }
}

bool check_move_to_shift_fn(GameInfoExtended_t *game_info) {
  bool res = false;
  if (!game_info->timer_activated) {
    game_info->timer = current_time_millis();
    game_info->timer_activated = true;
  }
  long long unsigned int lvl_speed = 1000 / game_info->game_inf.level;
  if (time_diff_fn(game_info->timer) >= lvl_speed &&
      game_info->state != EXIT_STATE && game_info->game_inf.pause != 1) {
    game_info->timer_activated = false;
    game_info->timer = 0;
    res = true;
  }
  return res;
}

bool check_shifting_to_attaching(GameInfoExtended_t *game_info) {
  bool res = false;
  if (check_collision_figure(game_info->move_field,
                                game_info->touched_field) ||
      check_collision_bottom(game_info->move_field)) {
    res = true;
  }
  return res;
}

void move_block(GameInfoExtended_t *game_info) {
  ++game_info->figure.x;
  block_to_move_field(game_info);
}

void move_from_move_to_touch(GameInfoExtended_t *game_info) {
  for (int i = 0; i != ROWS_MAP; ++i) {
    for (int j = 0; j != COLS_MAP; ++j) {
      if (game_info->move_field[i][j] == 1) {
        game_info->touched_field[i][j] = 1;
        game_info->move_field[i][j] = 0;
      }
    }
  }
}

void count_game_score(GameInfoExtended_t *game_info) {
  int filled_lines = 0;
  if ((filled_lines = check_filled_line(game_info->touched_field))) {
    if (filled_lines == 1) {
      game_info->game_inf.score += 100;
    }
    if (filled_lines == 2) {
      game_info->game_inf.score += 300;
    }
    if (filled_lines == 3) {
      game_info->game_inf.score += 700;
    }
    if (filled_lines == 4) {
      game_info->game_inf.score += 1500;
    }
  }
  if (game_info->game_inf.score > game_info->game_inf.high_score) {
    game_info->game_inf.high_score = game_info->game_inf.score;
    save_max_score(game_info);
  }
  set_lvl_fn(game_info);
}

void dealloc_field(int ***field, int rows) {
  if (*field != NULL) {
    for (int row = 0; row != rows; ++row) {
      if ((*field)[row] != NULL) {
        free((*field)[row]);
      }
    }
    free(*field);
  }
  *field = NULL;
}

static unsigned long long time_diff_fn(unsigned long long timer) {
  return current_time_millis() - timer;
}

static unsigned long long current_time_millis() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (unsigned long long)(tv.tv_sec) * 1000 +
         (unsigned long long)(tv.tv_usec) / 1000;
}

static void save_max_score(GameInfoExtended_t *game_info) {
  FILE *file = NULL;
  file = fopen("high_score.txt", "w");
  if (file) {
    fprintf(file, "%d", game_info->game_inf.high_score);
    fclose(file);
  }
}

static void load_max_score(GameInfoExtended_t *game_info) {
  FILE *file = NULL;
  int high_score = 0;
  file = fopen("high_score.txt", "r");
  if (file) {
    fscanf(file, "%d", &high_score);
    game_info->game_inf.high_score = high_score;
    fclose(file);
  }
}

static void fill_zero_fn(int **field, int rows, int cols) {
  for (int i = 0; i != rows; ++i) {
    for (int j = 0; j != cols; ++j) {
      field[i][j] = 0;
    }
  }
}

static void alloc_field_fn(int ***field, int rows, int cols) {
  *field = (int **)malloc(rows * sizeof(int *));
  if (*field != NULL) {
    for (int row = 0; row != rows; ++row) {
      (*field)[row] = (int *)malloc(cols * sizeof(int));
    }
    fill_zero_fn(*field, rows, cols);
  }
}

static void fill_next_fn(GameInfo_t *game_inf, int next[][4]) {
  for (int i = 0; i != 4; ++i) {
    for (int j = 0; j != 4; ++j) {
      game_inf->next[i][j] = next[i][j];
    }
  }
}

static void reset_static_field(int static_field[][COLS_MAP]) {
  for (int i = 0; i != ROWS_MAP; ++i) {
    for (int j = 0; j != COLS_MAP; ++j) {
      static_field[i][j] = 0;
    }
  }
}

static bool check_collision_bottom(int move_field[][COLS_MAP]) {
  for (int j = 0; j != COLS_MAP; ++j) {
    if (move_field[19][j] == 1) {
      return true;
    }
  }
  return false;
}

static bool check_collision_left(GameInfoExtended_t *game_info) {
  bool res = false;
  if (game_info->figure.y == 0) {
    for (int i = 0; i != 4; ++i) {
      if (game_info->figure.block[i][0] == 1) {
        res = true;
      }
    }
  }
  if (game_info->figure.y == -1) {
    for (int i = 0; i != 4; ++i) {
      if (game_info->figure.block[i][1] == 1) {
        res = true;
      }
    }
  }
  for (int i = 0; i != ROWS_MAP; ++i) {
    for (int j = 1; j != COLS_MAP; ++j) { // -1 проверяем j+1 и превышение будет
      if (game_info->move_field[i][j] == 1 &&
          game_info->touched_field[i][j - 1] == 1) {
        res = true;
      }
    }
  }
  return res;
}

static bool check_collision_right(GameInfoExtended_t *game_info) {
  bool res = false;
  if (game_info->figure.y == 6) {
    for (int i = 0; i != 4; ++i) {
      if (game_info->figure.block[i][3] == 1) {
        res = true;
      }
    }
  }
  if (game_info->figure.y == 7) {
    for (int i = 0; i != 4; ++i) {
      if (game_info->figure.block[i][2] == 1) {
        res = true;
      }
    }
  }
  if (game_info->figure.y == 8) {
    for (int i = 0; i != 4; ++i) {
      if (game_info->figure.block[i][1] == 1) {
        res = true;
      }
    }
  }
  for (int i = 0; i != ROWS_MAP; ++i) {
    for (int j = 0; j != COLS_MAP - 1; ++j) {// -1 проверяем j+1 и превышение будет
      if (game_info->move_field[i][j] == 1 &&
          game_info->touched_field[i][j + 1] == 1) {
        res = true;
      }
    }
  }
  return res;
}

static bool check_collision_figure(int move_field[][COLS_MAP],
                                      int touched_field[][COLS_MAP]) {
  bool res = false;
  for (int i = 0; i != ROWS_MAP; ++i) {
    for (int j = 0; j != COLS_MAP; ++j) {
      if (move_field[i][j] == 1 && touched_field[i + 1][j] == 1) {
        res = true;
      }
    }
  }
  return res;
}

static void delete_filled_line(int touched_field[][COLS_MAP], int line) {
  for (int j = 0; j != COLS_MAP; ++j) {
    touched_field[line][j] = 0;
  }
  for (int i = line; i != 0; --i) {
    for (int j = 0; j != COLS_MAP; ++j) {
      touched_field[i][j] = touched_field[i - 1][j];
    }
  }
}

static int check_filled_line(int touched_field[][COLS_MAP]) {
  int filled_lines = 0;
  int summ = 0;
  for (int i = 19; i > 0; i--) {
    for (int j = 0; j < COLS_MAP; ++j) {
      summ += touched_field[i][j];
      if (summ == 10) {
        delete_filled_line(touched_field, i);
        filled_lines += 1;
        ++i; 
      }
    }
    summ = 0;
  }
  return filled_lines;
}

static void set_lvl_fn(GameInfoExtended_t *game_info) {
  int lvl = game_info->game_inf.level;
  int score = game_info->game_inf.score;
  if (lvl < 11 && (score > (lvl * 600))) {
    game_info->game_inf.level++;
  }
}

static void turn_stick_fn(Figure *figure) {
  int turned_figure[4][4] = {0};
  for (int i = 0; i != 4; ++i) {
    for (int j = 0; j != 4; ++j) {
      turned_figure[j][i] = figure->block[i][j];
    }
  }
  for (int i = 0; i != 4; ++i) {
    for (int j = 0; j != 4; ++j) {
      figure->block[i][j] = turned_figure[i][j];
    }
  }
}

static void turn_other_fn(Figure *figure) {
  int turned_figure[3][3] = {0};
  for (int i = 0; i != 3; ++i) {
    for (int j = 0; j != 3; ++j) {
      int temp_j = j - 1;
      int temp_i = i - 1;
      int needed_i = 0;
      int needed_j = 0;
      needed_i = temp_j;
      needed_j = -temp_i;
      turned_figure[needed_i + 1][needed_j + 1] = figure->block[i][j];
    }
  }
  for (int i = 0; i != 3; ++i) {
    for (int j = 0; j != 3; ++j) {
      figure->block[i][j] = turned_figure[i][j];
    }
  }
}

static bool check_tetr_collision_after_action(GameInfoExtended_t *game_info) {
  bool res = false;
  for (int i = 0; i != ROWS_MAP; ++i) {
    for (int j = 0; j < COLS_MAP; ++j) {
      if (game_info->move_field[i][j] == 1 &&
          game_info->touched_field[i][j] == 1) {
        res = true;
      }
    }
  }
  return res;
}

static void cp_tetr_to_tetr(Figure *dst, Figure *src) {
  for (int i = 0; i != 4; ++i) {
    for (int j = 0; j != 4; ++j) {
      dst->block[i][j] = src->block[i][j];
    }
  }
  dst->x = src->x;
  dst->y = src->y;
  dst->current_type = src->current_type;
  dst->next_type = src->next_type;
}

static void check_y_after_action(GameInfoExtended_t *game_info) {
  int y = game_info->figure.y;
  if (y == -1) {  // y - координата левого верхнего угла
    for (int i = 0; i != 4; ++i) {
      if (game_info->figure.block[i][0] == 1) {
        game_info->figure.y++;
      }
    }
  }
  if (y == 7) {
    for (int i = 0; i != 4; ++i) {
      if (game_info->figure.block[i][3] == 1) {
        --game_info->figure.y;
      }
    }
  }
  if (y == 8) {
    for (int i = 0; i != 4; ++i) {
      if (game_info->figure.block[i][3] == 1) {
        --game_info->figure.y;
      }
    }
  }
  if (y == 8) {
    for (int i = 0; i != 4; ++i) {
      if (game_info->figure.block[i][2] == 1) {
        --game_info->figure.y;
      }
    }
  }
}

static bool check_x_after_action(GameInfoExtended_t *game_info) {
  bool res = false;
  int x = game_info->figure.x;
  if (x == 17) {
    for (int j = 0; j != 4; ++j) {
      if (game_info->figure.block[3][j] == 1) {
        res = true;
      }
    }
  }
  if (x == 18) {
    for (int j = 0; j != 4; ++j) {
      if (game_info->figure.block[2][j] == 1) {
        res = true;
      }
    }
  }
  if (x == 19) {
    for (int j = 0; j != 4; ++j) {
      if (game_info->figure.block[1][j] == 1) {
        res = true;
      }
    }
  }
  return res;
}
