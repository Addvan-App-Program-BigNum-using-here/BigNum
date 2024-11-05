CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -I..
LDFLAGS =

TEST_SRC = ../util.c ../msg_control.c ../random.c ../data_type.c ../operate.c ../file_io.c ./util_test.c
TEST_TARGET = util_test
# 추후 main 함수 생성되면 test 파일과 별도로 make 구성하기

all: $(TEST_TARGET)

#$(MAIN_TARGET):
#	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(MAIN_SRC)

$(TEST_TARGET):
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(TEST_SRC)

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
	cat ./result.txt

rerun: rebuild run
retest: rebuild test
rebuild: clean all

clean:
	rm -f $(TEST_TARGET)
	rm -f ../../sage_test/test.sage.py
	rm -f ./result.txt
	touch ./result.txt

.PHONY: all clean run rebuild