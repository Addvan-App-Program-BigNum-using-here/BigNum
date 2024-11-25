# Compiler and flags
CC = gcc
AR = ar
RANLIB = ranlib
# Directories
LIB_DIR = ../lib
# Library source files
LIB_SOURCES = ../util.c ../msg_control.c ../random.c ../data_type.c ../operate.c ../file_io.c
LIB_OBJECTS = $(LIB_SOURCES:../%.c=$(LIB_DIR)/%.o)
# Test files
TEST_SOURCE = util_test.c
TEST_OBJECT = $(TEST_SOURCE:.c=.o)
TEST_PROGRAM = util_test$(EXE_EXT)
TEST_SRC = $(LIB_SOURCES) ./$(TEST_SOURCE)
# Library names
SHARED_LIB = $(LIB_DIR)/$(SHARED_PREFIX)bignum.$(SHARED_EXT)
STATIC_LIB = $(LIB_DIR)/libbignum.a
# Sage test file
TEST_SAGE = ../../sage_test/test.sage.py
# Detect operating system
ifeq ($(OS),Windows_NT)
    # Windows settings
    SHARED_EXT = dll
    SHARED_FLAGS = -shared
    SHARED_PREFIX =
    EXE_EXT = .exe
    RM = del /Q /F
    CP = copy
    MKDIR = mkdir
    CFLAGS = -Wall -Wextra -std=c99 -I..
    TEST_RUN_CMD = set "PATH=$(LIB_DIR);$(PATH)" && .\$(TEST_PROGRAM)
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        # Linux settings
        SHARED_EXT = so
        SHARED_FLAGS = -shared -fPIC
        SHARED_PREFIX = lib
        EXE_EXT =
        RM = rm -f
        CP = cp
        MKDIR = mkdir -p
        CFLAGS = -Wall -Wextra -std=c99 -fsanitize=address -I..
        LDFLAGS = -g -fsanitize=address -Wl,-rpath,'$$ORIGIN/../lib'
        TEST_RUN_CMD = LD_LIBRARY_PATH=../lib ./$(TEST_PROGRAM)
    else ifeq ($(UNAME_S),Darwin)
        # macOS settings
        SHARED_EXT = dylib
        SHARED_FLAGS = -dynamiclib -fPIC
        SHARED_PREFIX = lib
        EXE_EXT =
        RM = rm -f
        CP = cp
        MKDIR = mkdir -p
        UNAME_M := $(shell uname -m)
        ifeq ($(UNAME_M),x86_64)
            CFLAGS = -Wall -Wextra -std=c99 -fsanitize=address -arch x86_64 -I..
            LDFLAGS = -g -fsanitize=address -arch x86_64 -Wl,-rpath,@executable_path/../lib
        else ifeq ($(UNAME_M),arm64)
            CFLAGS = -Wall -Wextra -std=c99 -fsanitize=address -arch arm64 -I..
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
$(TEST_OBJECT): $(TEST_SOURCE)
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
	$(CC) -o $(TEST_PROGRAM) $(TEST_OBJECT) -L$(LIB_DIR) -lbignum $(LDFLAGS)

# Run test program
run: test
	$(TEST_RUN_CMD)

# Rebuild and test with memory leak check
retest: clean $(TEST_SRC)
	$(CC) $(CFLAGS) $(INCLUDES) $(TEST_SRC) -o $(TEST_PROGRAM) $(LDFLAGS)
ifeq ($(UNAME_S),Darwin)
	leaks --atExit -- ./$(TEST_PROGRAM)
else
	./$(TEST_PROGRAM)
endif
	sage ../../sage_test/test.sage >/dev/null 2>&1
	cat ./main_result.txt
# Rebuild and run
rerun: clean run
clean:
	@echo "Cleaning up..."
ifeq ($(OS),Windows_NT)
	@if exist "$(LIB_DIR)" $(RM) $(subst /,\,$(LIB_OBJECTS))
	@$(RM) $(subst /,\,$(TEST_OBJECT))
	@$(RM) $(subst /,\,$(TEST_PROGRAM))
	@$(RM) *.txt
	@$(RM) $(subst /,\,$(SHARED_LIB))
	@$(RM) $(subst /,\,$(STATIC_LIB))
	@$(RM) $(subst /,\,$(TEST_SAGE))
else
	$(RM) $(LIB_OBJECTS)
	$(RM) $(TEST_OBJECT)
	$(RM) $(TEST_PROGRAM)
	$(RM) *.txt
	$(RM) $(SHARED_LIB)
	$(RM) $(STATIC_LIB)
	$(RM) $(TEST_SAGE)
endif
# Debug target
debug:
	@echo "Operating System: $(OS)"
	@echo "SHARED_LIB: $(SHARED_LIB)"
	@echo "TEST_PROGRAM: $(TEST_PROGRAM)"
	@echo "LIB_OBJECTS: $(LIB_OBJECTS)"
	@echo "Current directory: $(shell pwd)"