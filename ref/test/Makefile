CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -I..
LDFLAGS =

SRCS = ../util.c ../msg_control.c ../random.c ../data_type.c ./util_test.c
TARGET = util_test

all: $(TARGET)

$(TARGET):
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(SRCS)

run: $(TARGET)
	leaks --atExit -- ./$(TARGET)

rerun: rebuild run

rebuild: clean all

clean:
	rm -f $(TARGET)

.PHONY: all clean run rebuild