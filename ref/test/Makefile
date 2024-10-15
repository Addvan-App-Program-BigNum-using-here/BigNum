CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -I..
LDFLAGS =

SRCS = ../util.c ../error_msg.c ../random.c ../data_type.c ./util_test.c
TARGET = util_test

all: $(TARGET)

$(TARGET):
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(SRCS)

clean:
	rm -f $(TARGET)

.PHONY: all clean