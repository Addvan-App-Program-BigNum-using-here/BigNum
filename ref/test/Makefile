TEST_SRC = ../util.c ../msg_control.c ../random.c ../data_type.c ../operate.c ../file_io.c ./util_test.c

# Detect operating system
ifeq ($(OS),Windows_NT)
    # Windows settings
    CC = gcc
    CFLAGS = -Wall -Wextra -std=c99 -I..
    LDFLAGS =
    INCLUDES = -I..
    TEST_TARGET = util_test.exe
else
    UNAME_S := $(shell uname -s)
    UNAME_M := $(shell uname -m)
    ifeq ($(UNAME_S),Darwin)
        # Mac settings
        CC = clang
        ifeq ($(UNAME_M),x86_64)
            CFLAGS = -Wall -Wextra -std=c99 -fsanitize=address -arch x86_64 -I..
            LDFLAGS = -g -fsanitize=address -arch x86_64
        else ifeq ($(UNAME_M),arm64)
            CFLAGS = -Wall -Wextra -std=c99 -fsanitize=address -arch arm64 -I..
            LDFLAGS = -g -fsanitize=address -arch arm64
        endif
    else
        # Linux settings (assuming arm64 architecture)
        CC = clang
        CFLAGS = -Wall -Wextra -std=c99 -fsanitize=address -I..
        LDFLAGS = -g -fsanitize=address
    endif
    INCLUDES = -I..
    TEST_TARGET = util_test
endif

all: $(TEST_TARGET)

$(TEST_TARGET): $(TEST_SRC)
	$(CC) $(CFLAGS) $(INCLUDES) $(TEST_SRC) -o $(TEST_TARGET)

ifeq ($(OS),Windows_NT)
run: $(TEST_TARGET)
	./$(TEST_TARGET)
else
run: $(TEST_TARGET)
	leaks --atExit --  ./$(TEST_TARGET)
endif

test: $(TEST_TARGET)
	./$(TEST_TARGET)
	cat ./main_result.txt

rerun: rebuild run
retest: rebuild test
rebuild: clean all

clean:
	@echo "Cleaning up..."
ifeq ($(OS),Windows_NT)
	del /F $(TEST_TARGET)
else
	rm -f $(TEST_TARGET)
	rm -f ../../sage_test/test.sage.py
	rm -f ./test.txt
	touch ./test.txt
endif

.PHONY: all clean run rebuild test rerun retest