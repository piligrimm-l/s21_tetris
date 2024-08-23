CC = gcc
CFLAGS = -Wall -Werror -Wextra -Wpedantic -std=c11
VALGRIND_FLAGS = --leak-check=full --show-leak-kinds=all --track-origins=yes
TET_LIB_C = ./brick_game/tetris/fsm.c ./brick_game/tetris/tetris_backend.c ./brick_game/tetris/tetris_lib.c
TET_LIB_O = fsm.o tetris_backend.o tetris_lib.o
CLI_C = ./gui/cli/cli.c
SUITE_CASES_C = suite_*.c
SUITE_CASES_O = suite_*.o
FLAG_TESTS = -lcheck
TESTS_C=test.c
TESTS_O=test.o
ifeq ($(shell uname), Linux)
	FLAG_TEST +=-lsubunit -lm -lcheck_pic
endif


all: clean uninstall install 

install:tetris_lib.a
	$(CC) $(CFLAGS) -o tetris_game tetris.c  gui/cli/cli.c tetris_lib.a -lncurses
	make clean

uninstall:
	-rm -f tetris_game 

tetris_lib.a:
	$(CC) -c $(CFLAGS) $(TET_LIB_C)  
	ar rc tetris_lib.a $(TET_LIB_O) -lm
	ranlib tetris_lib.a

test: clean tetris_lib.a
	gcc -g --coverage $(CFLAGS) $(TESTS_C) -o test tetris_lib.a -lcheck_pic -lm -lsubunit
	./test

gcov_report: test
	gcc --coverage $(TET_LIB_C) $(TESTS_C) -o greport -lcheck_pic -lm -lsubunit
	./greport
	lcov -t "test" -o tetris.info -c -d .
	genhtml -o ./report tetris.info #--exclude test.c
	xdg-open ./report/index.html

valgrind_test: tetris_lib.a
	$(CC) -c $(CFLAGS) $(TESTS_C) -g
	$(CC) -g $(CFLAGS) $(FLAG_COV) -o tests $(TET_LIB_C) $(TESTS_O) -lcheck_pic -lm -lsubunit 
	valgrind $(VALGRIND_FLAGS) ./tests

dvi:
	open ./README.md

dist:
	tar -czf tetris.install.tar.gz ./* 

cpp_check:
	cppcheck --enable=all --force $(TET_LIB_C) $(CLI_C) 

check_style:
	cp ../materials/linters/.clang-format ./
	clang-format --style=Google -n *.c *.h
	rm -rf .clang-format

fixed_style:
	cp ../materials/linters/.clang-format ./
	clang-format --style=Google -i *.c *.h
	rm -rf .clang-format

clean: 
	-rm -rf *.o *.html *.gcda *.gcno *.css *.a *.gcov *.info *.out *.cfg *.txt
	-rm -f test 
	-rm -f tests
	-rm -f greport
	-rm -rf report
	-rm -f tetris.install.tar.gz 
	find . -type d -name 'tests.dSYM' -exec rm -r {} +
