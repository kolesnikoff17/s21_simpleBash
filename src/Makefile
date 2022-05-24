CC=gcc
CFLAGS=-Wall -Wextra -Werror -std=c11
LDFLAGS:=

SOURCES:=cat/cat.c common/common.c grep/grep.c

OBJECTS = $(SOURCES:.c=.o)

.PHONY: all clean rebuild lint cat grep

all: cat grep

cat: common/common.o cat/cat.o
	$(CC) $(LDFLAGS) $^ -o $@/s21_$@

grep: common/common.o grep/grep.o
	$(CC) $(LDFLAGS) $^ -o $@/s21_$@

%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

rebuild: clean all

clean:
	rm -rf */*.o
	rm -f cat/s21_cat
	rm -f grep/s21_grep

lint:
	cp ../materials/linters/CPPLINT.cfg ./
	python3 ../materials/linters/cpplint.py --extensions=c *.h *.c
	rm -f CPPLINT.cfg
