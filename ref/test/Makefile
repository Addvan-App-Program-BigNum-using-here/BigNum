CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -I.. -I"C:/msys64/mingw64/include"
LDFLAGS = -L"C:/msys64/mingw64/lib" -lssl -lcrypto

SRCS = ../util.c ../error_msg.c ../random.c ../data_type.c ./util_test.c
TARGET = util_test

all: $(TARGET)

$(TARGET):
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(SRCS)

clean:
	rm -f $(TARGET)

.PHONY: all clean
