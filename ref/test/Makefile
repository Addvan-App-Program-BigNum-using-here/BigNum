CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -I..
LDFLAGS =

TEST_SRC = ../util.c ../msg_control.c ../random.c ../data_type.c ../operate.c ../file_io.c ./util_test.c
TEST_TARGET = util_test

all: $(TEST_TARGET)

$(TEST_TARGET):
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(TEST_SRC)

ifeq ($(OS),Windows_NT)
run: $(TEST_TARGET)
	$(TEST_TARGET)
else
run: $(TEST_TARGET)
	leaks --atExit -- ./$(TEST_TARGET)
endif

test : $(TEST_TARGET)
	leaks --atExit -- ./$(TEST_TARGET) test

rerun: rebuild run
retest: rebuild test
rebuild: clean all

clean:
	rm -f $(TEST_TARGET)
	rm -f ../../sage_test/test.sage.py
	rm -f ./result.txt
	touch ./result.txt

.PHONY: all clean run rebuild