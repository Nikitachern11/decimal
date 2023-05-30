UNAME := $(shell uname)
ifeq ($(UNAME), Linux)
FLAGS = -lm -lrt -lsubunit -lpthread
else
FLAGS =
endif

all: s21_decimal.a gcov_report

s21_decimal.a: s21_decimal.o
	ar rc s21_decimal.a s21_decimal.o
	ranlib s21_decimal.a

s21_decimal.o: s21_decimal.c
	gcc -Wall -Werror -Wextra -std=c11 -c s21_decimal.c


s21_decimal_test.o: 
	checkmk s21_test.check > s21_decimal_test.c
	gcc -c s21_decimal_test.c -g

test: s21_decimal.a s21_decimal_test.o
	gcc -Wall -Werror -Wextra s21_decimal_test.o s21_decimal.a -o tests -lcheck $(FLAGS)
	./tests

gcov_report: s21_decimal_test.o
	gcc -c -fprofile-arcs -ftest-coverage s21_decimal.c
	gcc -Wall -fprofile-arcs s21_decimal_test.o s21_decimal.o -o tests -lcheck $(FLAGS)
	./tests
	gcov -f s21_decimal.gcda
	gcovr -r . --html --html-details -o coverage_report.html
	rm s21_decimal.o
	open coverage_report.html


clean:
	rm -f *.o *.a   s21_decimal_test.c tests *.html s21_decimal.gcda s21_decimal.gcno
