CC = gcc
CFLAGS = -Wall -Wextra -std=c99
LDFLAGS = -lcrypto -lssl

# Source files
SRCS = util_test.c \
       ../util.c \
       ../msg_control.c \
       ../random.c \
       ../data_type.c \
       ../operate.c \
       ../file_io.c

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
    else
        # Linux settings
        INCLUDES = -I..
        LIBDIRS = 
        TARGET = util_test
    endif
endif

# Objects are all source files with .c replaced by .o
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

# Linking
$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LIBDIRS) $(LDFLAGS) -o $@

# Compilation
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

ifeq ($(OS),Windows_NT)
run: $(TARGET)
	./$(TARGET)
else
run: $(TARGET)
	leaks --atExit -- ./$(TARGET)
endif

test: $(TARGET)
	leaks --atExit -- ./$(TARGET)
	cat ./test.txt

clean:
	@echo "Cleaning up..."
ifeq ($(OS),Windows_NT)
	del /F $(TARGET) $(OBJS)
else
	rm -f $(TARGET) $(OBJS)
	rm -f ../../sage_test/test.sage.py
	rm -f ./test.txt
	touch ./test.txt
endif

.PHONY: all clean run test