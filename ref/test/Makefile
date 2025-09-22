# Compiler and flags
CC = gcc
AR = ar
RANLIB = ranlib

# Test files
TEST_SOURCE = test_main.c util_test.c random_test.c operate_test.c crypto_test.c func_cmp_test.c
TEST_OBJECT = $(TEST_SOURCE:.c=.o)

TEST_PROGRAM = test_main$(EXE_EXT)
TEST_SRC = $(LIB_SOURCES) ./$(TEST_SOURCE)
# Result directory
TEST_VEC = ./test_vec
# Detect operating system
ifeq ($(OS),Windows_NT)
    # Library names
    SHARED_LIB = $(LIB_DIR)\$(SHARED_PREFIX)HEPHAISTOS.$(SHARED_EXT)
    STATIC_LIB = $(LIB_DIR)\HEPHAISTOS.a
    # Windows settings
    RESULT_DIR = .\result
    LIB_DIR = ..\lib
    LIB_SOURCES = ..\util.c ..\msg_control.c ..\random.c ..\data_type.c ..\operate.c ..\file_io.c ..\crypto.c
    LIB_OBJECTS = $(LIB_SOURCES:..\%.c=$(LIB_DIR)\%.o)
    SHARED_EXT = dll
    SHARED_FLAGS = -shared
    SHARED_PREFIX =
    EXE_EXT = .exe
    RM = del /Q /F
    CP = cp 
    MKDIR = mkdir
    TOUCH = type nul >
    CFLAGS = -Wall -O3  -Wextra -std=c99 -I..
    TEST_RUN_CMD = set "PATH=$(LIB_DIR);$(PATH)" && ./$(TEST_PROGRAM)
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        # Library names
        SHARED_LIB = $(LIB_DIR)/$(SHARED_PREFIX)HEPHAISTOS.$(SHARED_EXT)
        STATIC_LIB = $(LIB_DIR)/HEPHAISTOS.a
        RESULT_DIR = ./result
        LIB_DIR = ../lib
        LIB_SOURCES = ../util.c ../msg_control.c ../random.c ../data_type.c ../operate.c ../file_io.c ../crypto.c
        LIB_OBJECTS = $(LIB_SOURCES:../%.c=$(LIB_DIR)/%.o)
        # Linux settings
        SHARED_EXT = so
        SHARED_FLAGS = -shared -fPIC
        SHARED_PREFIX = lib
        EXE_EXT =
        RM = rm -f
        CP = cp
        MKDIR = mkdir -p
        TOUCH = touch
        CFLAGS = -Wall -std=gnu99 -O3 -Wextra -std=c99 -fsanitize=address -I..
        LDFLAGS = -g -fsanitize=address -Wl,-rpath,'$$ORIGIN/../lib'
        TEST_RUN_CMD = LD_LIBRARY_PATH=../lib ./$(TEST_PROGRAM)
    else ifeq ($(UNAME_S),Darwin)
        # Library names# Library names
        SHARED_LIB = $(LIB_DIR)/$(SHARED_PREFIX)HEPHAISTOS.$(SHARED_EXT)
        STATIC_LIB = $(LIB_DIR)/HEPHAISTOS.a
        RESULT_DIR = ./result
        # macOS settings
        LIB_DIR = ../lib
        LIB_SOURCES = ../util.c ../msg_control.c ../random.c ../data_type.c ../operate.c ../file_io.c ../crypto.c
        LIB_OBJECTS = $(LIB_SOURCES:../%.c=$(LIB_DIR)/%.o)
        SHARED_EXT = dylib
        SHARED_FLAGS = -dynamiclib -fPIC
        SHARED_PREFIX = lib
        EXE_EXT =
        RM = rm -f
        CP = cp
        MKDIR = mkdir -p
        TOUCH = touch
        UNAME_M := $(shell uname -m)
        ifeq ($(UNAME_M),x86_64)
            CFLAGS = -Wall -O3 -Wextra -std=c99 -fsanitize=address -arch x86_64 -I..
            LDFLAGS = -g -fsanitize=address -arch x86_64 -Wl,-rpath,@executable_path/../lib
        else ifeq ($(UNAME_M),arm64)
            CFLAGS = -O3 -Wall -Wextra -std=c99 -fsanitize=address -arch arm64 -I..
            LDFLAGS = -g -fsanitize=address -arch arm64 -Wl,-rpath,@executable_path/../lib
        endif
        TEST_RUN_CMD = DYLD_LIBRARY_PATH=../lib ./$(TEST_PROGRAM)
    endif
endif
.PHONY: all clean shared test run rerun retest
all: clean $(LIB_DIR) shared run
$(LIB_DIR):
	$(MKDIR) $(LIB_DIR)
# Library object files
$(LIB_DIR)/%.o: ../%.c ../%.h | $(LIB_DIR)
	$(CC) $(CFLAGS) -c $< -o $@
# Test object files
$(TEST_OBJECT): %.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
# Create shared library
shared: $(LIB_OBJECTS)
	$(CC) $(SHARED_FLAGS) -o $(SHARED_LIB) $(LIB_OBJECTS) $(LDFLAGS)
ifeq ($(OS),Windows_NT)
	$(CP) $(SHARED_LIB) .\\
endif
# Create static library
static: $(LIB_OBJECTS)
	$(AR) rcs $(STATIC_LIB) $(LIB_OBJECTS)
	$(RANLIB) $(STATIC_LIB)
# Build test program
test: $(TEST_OBJECT) shared
	$(CC) -o $(TEST_PROGRAM) $(TEST_OBJECT) -L$(LIB_DIR) -lHEPHAISTOS $(LDFLAGS)
# Run test program
run: test
	$(TEST_RUN_CMD)
# Rebuild and test with memory leak check
retest: clean $(TEST_SRC)
	$(CC) $(CFLAGS) $(INCLUDES) $(TEST_SRC) -o $(TEST_PROGRAM) $(LDFLAGS)
ifeq ($(OS),Windows_NT)
	./$(TEST_PROGRAM)
	python3 ../../sage_test/test.py
	type main_result.txt
else ifeq ($(UNAME_S),Darwin)
	leaks --atExit -- ./$(TEST_PROGRAM)
	python3 ../../sage_test/test.py
	cat ./main_result.txt
else
	./$(TEST_PROGRAM)
	python3 ../../sage_test/test.py
	cat ./main_result.txt
endif
# Rebuild and run
rerun: clean run

clean:
	@echo "Cleaning up..."
ifeq ($(OS),Windows_NT)
	$(RM) $(TEST_OBJECT)
	$(RM) $(TEST_PROGRAM)
	$(RM) *.txt
	$(RM) $(RESULT_DIR)\*.txt
	$(RM) $(SHARED_LIB)
	$(RM) $(STATIC_LIB)
else
	$(RM) $(LIB_OBJECTS)
	$(RM) $(TEST_OBJECT)
	$(RM) $(TEST_PROGRAM)
	$(RM) *.txt
	$(RM) $(RESULT_DIR)/*.txt
	$(RM) $(SHARED_LIB)
	$(RM) $(STATIC_LIB)
endif
