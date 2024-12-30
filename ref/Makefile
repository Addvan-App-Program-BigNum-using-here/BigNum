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
	SRC_DIR = .\src\
	INC_DIR = .\include\
	BENCH_DIR = .\benchmark\
	SHARED_EXT = dll
	SHARED_FLAGS = -shared
	SHARED_PREFIX =
	EXE_EXT = .exe
	RM = del /Q /F
	CP = copy
	MKDIR = mkdir
	READ = type
else
	TEST_VEC = ./test_vec/
	PYTHON_DIR = ../sage_test/
	TEST_DIR = ./test/
	RESULT_DIR = $(TEST_DIR)result/
	LIB_DIR = ./lib/
	SRC_DIR = ./src/
	INC_DIR = ./include/
	BENCH_DIR = ./benchmark/
	SHARED_PREFIX = lib_
	EXE_EXT =
	RM = rm -f
	CP = cp
	MKDIR = mkdir -p
	READ = cat

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
SOURCES = $(SRC_DIR)util.c $(SRC_DIR)msg_control.c $(SRC_DIR)random.c $(SRC_DIR)data_type.c $(SRC_DIR)operate.c $(SRC_DIR)file_io.c $(SRC_DIR)crypto.c
SOURCES_BENCH = $(SOURCES) $(BENCH_DIR)bench_util.c
SOURCES_TEST = $(SOURCES) $(TEST_DIR)test_util.c

# Test files
TEST_PROGRAM = $(TEST_DIR)fix_crypto_test$(EXE_EXT) $(TEST_DIR)rand_crypto_test$(EXE_EXT) \
 			   $(TEST_DIR)fix_operate_test$(EXE_EXT) $(TEST_DIR)rand_operate_test$(EXE_EXT) \
 			   $(TEST_DIR)util_test$(EXE_EXT) $(TEST_DIR)func_cmp_test$(EXE_EXT)

# Bench files
BENCH_PROGRAM = $(BENCH_DIR)bignum_benchmark$(EXE_EXT) $(BENCH_DIR)crypto_util_benchmark$(EXE_EXT)


# Library Set
SHARED_LIB = $(LIB_DIR)$(SHARED_PREFIX)HEPHAISTOS.$(SHARED_EXT)
STATIC_LIB = $(LIB_DIR)HEPHAISTOS.a
LIB_OBJECTS = $(SOURCES:$(SRC_DIR)%.c=$(LIB_DIR)%.o)

# Compiler flags
INCLUDES = -I$(INC_DIR)
INCLUDES_BENCH = $(INCLUDES) -I$(BENCH_DIR)
INCLUDES_TEST = $(INCLUDES) -I$(TEST_DIR)

CFLAGS = -Wall -O3 -Wextra -std=c99
LDFLAGS =

# Detect operating system
ifeq ($(OS),Windows_NT)
    RUN_CMD = set "PATH=$(LIB_DIR);$(PATH)" && $(TEST_PROGRAM)
    TEST_RUN_CMD = $(TEST_PROGRAM)
    CFLAGS += -std=c99
    LDFLAGS =
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        CFLAGS += -std=gnu99 -fsanitize=address
        LDFLAGS += -Wl,-rpath,'$$ORIGIN/../lib'
        RUN_CMD = LD_LIBRARY_PATH=../lib $(TEST_PROGRAM)
        TEST_RUN_CMD = $(TEST_PROGRAM)
    else ifeq ($(UNAME_S),Darwin)
    	UNAME_M := $(shell uname -m)
        CFLAGS += -arch $(UNAME_M)
        LDFLAGS +=  -arch $(UNAME_M) -Wl,-rpath,@executable_path/../lib
        RUN_CMD = DYLD_LIBRARY_PATH=../lib $(TEST_PROGRAM)
        TEST_RUN_CMD = leaks --atExit -- $(TEST_PROGRAM)
    endif
endif

# -------------------------------------------------- env setting -------------------------------------------------------

.PHONY: all clean shared run test
all: clean $(LIB_DIR) shared run

# -------------------------------------------------- library make and run ----------------------------------------------
lib: clean $(LIB_DIR) shared

lib_run: lib run

$(LIB_DIR):
	@echo "Creating library directory..."
	$(MKDIR) $(LIB_DIR)

# Library object files
$(LIB_DIR)%.o: $(SRC_DIR)%.c $(INC_DIR)%.h | $(LIB_DIR)
	@ echo "Compiling $<..."
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Create shared library
shared: $(LIB_OBJECTS) # depend on all object files
	$(CC) $(SHARED_FLAGS) -o $(SHARED_LIB) $(LIB_OBJECTS) $(LDFLAGS)
ifeq ($(OS),Windows_NT)
	$(CP) $(SHARED_LIB) .\\
endif

# Create static library
static: $(LIB_OBJECTS)
	$(AR) rcs $(STATIC_LIB) $(LIB_OBJECTS)
	$(RANLIB) $(STATIC_LIB)

# Run test program
run:
	$(CC) $(CFLAGS) -DTEST_MODE=0 -DSIZE_MODE=0 $(INCLUDES) -o $(TEST_PROGRAM) $(TEST_OBJECT) -L$(LIB_DIR) -lHEPHAISTOS $(LDFLAGS)
	$(RUN_CMD)

# -------------------------------------------------- test make and run ----------------------------------------------
# When test start then clean and make library and run python test code
test_build: clean lib


#	# create fix input crypto test
#	$(CC) $(CFLAGS) -DTEST_MODE=0 -DSIZE_MODE=0 $(INCLUDES_TEST) $(SOURCES_TEST) $(TEST_DIR)crypto_test.c -o $(TEST_DIR)fix_crypto_test
#	# create fix input operate test
#	$(CC) $(CFLAGS) -DTEST_MODE=0 -DSIZE_MODE=0 $(INCLUDES_TEST) $(SOURCES_TEST) $(TEST_DIR)operate_test.c -o $(TEST_DIR)fix_operate_test
#
#	# create random input crypto test
#	$(CC) $(CFLAGS) -DTEST_MODE=1 -DSIZE_MODE=1 $(INCLUDES_TEST) $(SOURCES_TEST) $(TEST_DIR)crypto_test.c -o $(TEST_DIR)rand_crypto_test
#	# create random input operate test
#	$(CC) $(CFLAGS) -DTEST_MODE=1 -DSIZE_MODE=1 $(INCLUDES_TEST) $(SOURCES_TEST) $(TEST_DIR)operate_test.c -o $(TEST_DIR)rand_operate_test
#
#	# create fix input util test
#	$(CC) $(CFLAGS) $(INCLUDES_TEST) $(SOURCES_TEST) $(TEST_DIR)util_test.c -o $(TEST_DIR)util_test
#	# create fix input func_cmp test
#	$(CC) $(CFLAGS) $(INCLUDES_TEST) $(SOURCES_TEST) $(TEST_DIR)func_cmp_test.c -o $(TEST_DIR)func_cmp_test

#	$(TEST_RUN_CMD)
#	python3 $(PYTHON_DIR)test.py
#	$(READ) ./result.txt

# -------------------------------------------------- benchmark make and run ----------------------------------------------

bench : clean
	$(CC) $(CFLAGS) $(INCLUDES_BENCH) $(SOURCES_BENCH) $(BENCH_DIR)bignum_benchmark.c -o ./bignum_benchmark
	$(CC) $(CFLAGS) $(INCLUDES_BENCH) $(SOURCES_BENCH) $(BENCH_DIR)crypto_util_benchmark.c -o ./crypto_util_benchmark

# -------------------------------------------------- clean up ----------------------------------------------------------
clean:
	@echo "Cleaning up..."
	$(RM) $(LIB_OBJECTS)
	$(RM) $(TEST_PROGRAM)
	$(RM) *.txt
	$(RM) $(RESULT_DIR)*.txt
	$(RM) $(SHARED_LIB)
	$(RM) $(STATIC_LIB)
	$(RM) $(TEST_PROGRAM)
	$(RM) $(BENCH_PROGRAM)