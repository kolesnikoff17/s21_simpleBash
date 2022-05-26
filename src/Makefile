CC=gcc
CFLAGS=-Wall -Wextra -Werror -std=c11
LDFLAGS:=

SOURCES:=cat/s21_cat.c grep/s21_grep.c

OBJECTS = $(SOURCES:.c=.o)

.PHONY: all clean rebuild lint s21_cat s21_grep

all: s21_cat s21_grep

s21_cat: cat/s21_cat.o
	$(CC) $(LDFLAGS) $^ -o cat/$@

s21_grep: grep/s21_grep.o
	$(CC) $(LDFLAGS) $^ -o grep/$@

%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

rebuild: clean all

clean:
	rm -rf */*.o
	rm -f cat/s21_cat
	rm -f grep/s21_grep
	rm -f */0*

lint:
	cp ../materials/linters/CPPLINT.cfg ./
	python3 ../materials/linters/cpplint.py --extensions=c */*.h */*.c
	rm -f CPPLINT.cfg
