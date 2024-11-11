CC = clang

TEST_SRC = ../util.c ../msg_control.c ../random.c ../data_type.c ../operate.c ../file_io.c ./util_test.c

# Detect operating system
ifeq ($(OS),Windows_NT)
    # Windows settings
    CFLAGS = -Wall -Wextra -std=c99 -fsanitize=address -I..
    LDFLAGS = -fsanitize=address
    INCLUDES = -I..
    TEST_TARGET = util_test.exe
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Darwin)
        # Mac settings
        CFLAGS = -Wall -Wextra -std=c99 -fsanitize=address -arch arm64 -I..
        LDFLAGS = -g -fsanitize=address -arch arm64
        INCLUDES = -I..
        TEST_TARGET = util_test
    endif
endif

all: $(TEST_TARGET)

$(TEST_TARGET): $(TEST_SRC)
	$(CC) $(CFLAGS) $(INCLUDES) $(TEST_SRC) -o $(TEST_TARGET)

ifeq ($(OS),Windows_NT)
run: $(TEST_TARGET)
	$(TEST_TARGET)
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
	rm -f $(TARGET)
else
	rm -f $(TEST_TARGET)
	rm -f ../../sage_test/test.sage.py
	rm -f ./main_result.txt
	touch ./main_result.txt
endif

.PHONY: all clean run rebuild test rerun retest