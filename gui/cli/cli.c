#include "cli.h"

static void print_border();
static void print_stats(GameInfo_t game);
static void print_field(GameInfo_t game);
static void print_game_over();
static void print_pause();
static void print_start_screen();
static void print_control_keys();

void frontend_general_fn(GameInfo_t game) {
  clear();
  if (game.field == NULL) {
    print_start_screen();
  } else {
    print_stats(game);
    print_field(game);
  }
  if (game.pause == 1) {
    print_pause();
  }
  if (game.pause == 2) {
    print_game_over();
  }
  print_control_keys();
  print_border();
  refresh();
}

void enable_ncurses() {
  initscr();
  curs_set(0);
  cbreak();
  halfdelay(1);
  keypad(stdscr, TRUE);
}

void disable_ncurses() { endwin(); }

static void print_border() {
  const char border = '|';
  int row = 22, col = 22;
  for (int i = 0; i != row; ++i) {
    for (int j = 0; j != col; ++j) {
      if (i == 0) {
        mvaddch(i, j, border);
      }
      if (i == row - 1) {
        mvaddch(i, j, border);
      }
      if (j == 0 && i > 0) {
        mvaddch(i, j, border);
      }
      if (j == col - 1 && i > 0) {
        mvaddch(i, j, border);
      }
    }
  }
}

static void print_stats(GameInfo_t game) {
  mvaddstr(0, 24, "Score");
  mvprintw(1, 24, "%d", game.score);

  mvaddstr(2, 24, "High Score");
  mvprintw(3, 24, "%d", game.high_score);

  mvaddstr(4, 24, "Level");
  mvprintw(5, 24, "%d", game.level);

  mvaddstr(6, 24, "Speed");
  mvprintw(7, 24, "%d", game.speed);

  mvaddstr(9, 25, "Next");
  for (int i = 0; i != 2; ++i) {
    for (int j = 0; j != 4; ++j) {
      if (game.next[i][j] == 1) {
        mvaddch(i + 11, j * 2 + 24, '[');
        mvaddch(i + 11, j * 2 + 25, ']');
      }
    }
  }
}

static void print_field(GameInfo_t game) {
  for (int i = 0; i != 20; ++i) {
    for (int j = 0; j != 10;++j) {
      if (game.field[i][j] == 1) {
        mvaddch(i + 1, j * 2 + 1, '[');
        mvaddch(i + 1, j * 2 + 2, ']');
      }
    }
  }
}

static void print_start_screen() {
  mvaddstr(10, 2, "Press 's' to start");
}

static void print_control_keys() {
  mvaddstr(14, 24, "s - start");
  mvaddstr(15, 24, "q - exit");
  mvaddstr(16, 24, "space - action");
  mvaddstr(17, 24, "p - pause, unpause");
  mvaddstr(18, 24, "<-,->  - left and right move");
  mvaddstr(19, 24, "down arrow  - fast move down");
}

static void print_game_over() {
  mvaddstr(9, 1, "____________________");
  mvaddstr(10, 7, "GAME OVER");
  mvaddstr(11, 1, "____________________");
  mvaddstr(12, 1, "Press 's' to restart");
  mvaddstr(13, 1, "____________________");
}

static void print_pause() {
  mvaddstr(9, 1, "____________________");
  mvaddstr(10, 8, "PAUSE");
  mvaddstr(11, 1, "____________________");
  mvaddstr(12, 1, "Press 'p' to resume");
  mvaddstr(13, 1, "____________________");
}
