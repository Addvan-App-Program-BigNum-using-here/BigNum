CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -I.. -I"C:/msys64/mingw64/include"
LDFLAGS = -L"C:/msys64/mingw64/lib" -lssl -lcrypto

TEST_SRC = ../util.c ../msg_control.c ../random.c ../data_type.c ../operate.c ../file_io.c ./util_test.c
TEST_TARGET = util_test

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

all: $(TEST_TARGET)

#$(MAIN_TARGET):
#	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(MAIN_SRC)

$(TEST_TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(INCLUDES) $(LIBDIRS) $(SRCS) $(LIBS) -o $(TARGET)

ifeq ($(OS),Windows_NT)
run: $(TEST_TARGET)
	$(TEST_TARGET)
else
# main 함수 실행
run: $(TEST_TARGET)
	leaks --atExit -- ./$(TEST_TARGET)
endif

# test 함수 실행
test : $(TEST_TARGET)
	leaks --atExit -- ./$(TEST_TARGET)
	cat ./main_result.txt

rerun: rebuild run
retest: rebuild test
rebuild: clean all
  
clean:
	@echo "Cleaning up..."
ifeq ($(OS),Windows_NT)
	rm -f $(TARGET)
else
	rm -f $(TEST_TARGET)
	rm -f ../../sage_test/test.sage.py
	rm -f ./main_result.txt
	touch ./main_result.txt
endif

.PHONY: all clean run rebuild

