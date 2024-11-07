CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -I.. -I"C:/msys64/mingw64/include"
LDFLAGS = -L"C:/msys64/mingw64/lib" -lssl -lcrypto

SRCS = ../util.c ../error_msg.c ../random.c ../data_type.c ./util_test.c

# Detect operating system
ifeq ($(OS),Windows_NT)
    # Windows settings
    INCLUDES = -I.. -I"C:/msys64/mingw64/include"
    LIBDIRS = -L"C:/msys64/mingw64/lib"
    TARGET = util_test.exe
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Darwin)
        # Mac settings
        INCLUDES = -I.. -I/usr/local/opt/openssl/include
        LIBDIRS = -L/usr/local/opt/openssl/lib
        TARGET = util_test
    endif
endif
# Libraries
LIBS = -lcrypto -lssl

all: $(TARGET)


$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(INCLUDES) $(LIBDIRS) $(SRCS) $(LIBS) -o $(TARGET)
clean:
	@echo "Cleaning up..."
ifeq ($(OS),Windows_NT)
	rm -f $(TARGET)
else
	rm -f $(TARGET)
endif

.PHONY: all clean

