# Compiler and flags
CC = gcc
AR = ar
RANLIB = ranlib

# Variation Set for Windows and Linux and Mac
ifeq ($(OS),Windows_NT)
	TEST_VEC = .\test_vec\
	PYTHON_DIR = ..\sage_test\
	TEST_DIR = .\test\
	RESULT_DIR = $(TEST_DIR)result\
	LIB_DIR = .\lib\
	SRCDIR = .\src\
	INCDIR = .\include\
	SHARED_EXT = dll
	SHARED_FLAGS = -shared
	SHARED_PREFIX =
	EXE_EXT = .exe
	RM = del /Q /F
	CP = copy
	MKDIR = mkdir
else
	TEST_VEC = ./test_vec/
	PYTHON_DIR = ../sage_test/
	TEST_DIR = ./test/
	RESULT_DIR = $(TEST_DIR)result/
	LIB_DIR = ./lib/
	SRCDIR = ./src/
	INCDIR = ./include/
	SHARED_PREFIX = lib
	EXE_EXT =
	RM = rm -f
	CP = cp
	MKDIR = mkdir -p

	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Linux)
		SHARED_EXT = so
		SHARED_FLAGS = -shared -fPIC
	else ifeq ($(UNAME_S),Darwin)
		SHARED_EXT = dylib
		SHARED_FLAGS = -dynamiclib -fPIC
	endif
endif

# Source files
SOURCES = $(SRCDIR)util.c $(SRCDIR)msg_control.c $(SRCDIR)random.c $(SRCDIR)data_type.c $(SRCDIR)operate.c $(SRCDIR)file_io.c $(SRCDIR)crypto.c

# Header files
HEADERS = $(INCDIR)util.h $(INCDIR)msg_control.h $(INCDIR)random.h $(INCDIR)data_type.h $(INCDIR)operate.h $(INCDIR)file_io.h $(INCDIR)crypto.h

# Test files
TEST_SOURCE = $(TEST_DIR)test_main.c $(TEST_DIR)util_test.c $(TEST_DIR)random_test.c $(TEST_DIR)operate_test.c $(TEST_DIR)crypto_test.c $(TEST_DIR)func_cmp_test.c
TEST_OBJECT = $(TEST_SOURCE:.c=.o)
TEST_PROGRAM = $(TEST_DIR)test_main$(EXE_EXT)
TEST_SRC = $(SOURCES) ./$(TEST_SOURCE)

# Library Set
SHARED_LIB = $(LIB_DIR)$(SHARED_PREFIX)HEPHAISTOS.$(SHARED_EXT)
STATIC_LIB = $(LIB_DIR)HEPHAISTOS.a
LIB_OBJECTS = $(SOURCES:$(SRCDIR)%.c=$(LIB_DIR)%.o)

# Compiler flags
INCLUDES = -I$(INCDIR)
CFLAGS = -Wall -O3 -Wextra -std=c99
LDFLAGS = -fsanitize=address

# Detect operating system
ifeq ($(OS),Windows_NT)
    TEST_RUN_CMD = set "PATH=$(LIB_DIR);$(PATH)" && .\$(TEST_PROGRAM)
    CFLAGS += -std=c99
    LDFLAGS =
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        CFLAGS += -std=gnu99 -fsanitize=address
        LDFLAGS += -Wl,-rpath,'$$ORIGIN/../lib'
        TEST_RUN_CMD = LD_LIBRARY_PATH=../lib ./$(TEST_PROGRAM)
    else ifeq ($(UNAME_S),Darwin)
    	UNAME_M := $(shell uname -m)
        LIB_OBJECTS = $(SOURCES:$(SRCDIR)%.c=$(LIB_DIR)%.o)
        CFLAGS += -fsanitize=address -arch $(UNAME_M)
        LDFLAGS +=  -arch $(UNAME_M) -Wl,-rpath,@executable_path/../lib
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
	python3 $(PYTHON_DIR)test.py
	type ./result.txt
else ifeq ($(UNAME_S),Darwin)
	leaks --atExit -- ./$(TEST_PROGRAM)
	python3 $(PYTHON_DIR)test.py
	cat ./result.txt
else
	./$(TEST_PROGRAM)
	python3 $(PYTHON_DIR)test.py
	cat ./result.txt
endif
# Rebuild and run
rerun: clean run

clean:
	@echo "Cleaning up..."
ifeq ($(OS),Windows_NT)
	$(RM) $(TEST_OBJECT)
	$(RM) $(TEST_PROGRAM)
	$(RM) *.txt
	$(RM) $(RESULT_DIR)*.txt
	$(RM) $(SHARED_LIB)
	$(RM) $(STATIC_LIB)
else
	$(RM) $(LIB_OBJECTS)
	$(RM) $(TEST_OBJECT)
	$(RM) $(TEST_PROGRAM)
	$(RM) *.txt
	$(RM) $(RESULT_DIR)*.txt
	$(RM) $(SHARED_LIB)
	$(RM) $(STATIC_LIB)
endif
