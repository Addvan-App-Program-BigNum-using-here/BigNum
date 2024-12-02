# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -I..

# Detect operating system
ifeq ($(OS),Windows_NT)
    # Windows settings
    SHARED_EXT = dll
    SHARED_FLAGS = -shared
    SHARED_PREFIX = lib
    EXE_EXT = .exe
    # LDFLAGS = -Wl,--out-implib,$(LIB_DIR)/libbignum.a
    RM = del /Q /F
    CP = copy
    MKDIR = mkdir
    # Windows needs current directory in PATH for DLLs
    TEST_RUN_CMD = set "PATH=$(LIB_DIR);$(PATH)" && .\util_test.exe
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        # Linux settings
        SHARED_EXT = so
        SHARED_FLAGS = -shared -fPIC
        SHARED_PREFIX = lib
        EXE_EXT =
        LDFLAGS = -Wl,-rpath,'$$ORIGIN/../lib'
        RM = rm -f
        CP = cp
        MKDIR = mkdir -p
        TEST_RUN_CMD = LD_LIBRARY_PATH=../lib ./util_test
    else ifeq ($(UNAME_S),Darwin)
        # macOS settings
        SHARED_EXT = dylib
        SHARED_FLAGS = -dynamiclib -fPIC
        SHARED_PREFIX = lib
        EXE_EXT =
        LDFLAGS = -Wl,-rpath,@executable_path/../lib
        RM = rm -f
        CP = cp
        MKDIR = mkdir -p
        TEST_RUN_CMD = DYLD_LIBRARY_PATH=../lib ./util_test
    endif
endif

# Directories
LIB_DIR = ../lib

# Source files with correct paths
SOURCES = ../util.c ../msg_control.c ../random.c ../data_type.c ../operate.c ../file_io.c
OBJECTS = $(SOURCES:../%.c=$(LIB_DIR)/%.o)

# Test files
TEST_SRC = util_test.c
TEST_OBJ = $(TEST_SRC:.c=.o)
TEST_TARGET = util_test$(EXE_EXT)

# Library names
SHARED_LIB = $(LIB_DIR)/$(SHARED_PREFIX)bignum.$(SHARED_EXT)
STATIC_LIB = $(LIB_DIR)/libbignum.a

.PHONY: all clean shared static test run

# Add -fPIC for all object files if not on Windows
ifneq ($(OS),Windows_NT)
    CFLAGS += -fPIC
endif

all: $(LIB_DIR) shared static test

$(LIB_DIR):
	$(MKDIR) $(LIB_DIR)

# Rule for object files
$(LIB_DIR)/%.o: ../%.c ../%.h | $(LIB_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Rule for test object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Create shared library
shared: $(OBJECTS)
	$(CC) $(SHARED_FLAGS) -o $(SHARED_LIB) $(OBJECTS) $(LDFLAGS)
ifeq ($(OS),Windows_NT)
	$(CP) $(SHARED_LIB) test\\
endif

# # Create static library
# static: $(OBJECTS)
# 	$(AR) rcs $(STATIC_LIB) $(OBJECTS)
# 	$(RANLIB) $(STATIC_LIB)

# Build test program
test: $(TEST_OBJ) shared
	$(CC) $(CFLAGS) -o $(TEST_TARGET) $(TEST_OBJ) -L$(LIB_DIR) -lbignum $(LDFLAGS)

# Run test program
run: test
	$(TEST_RUN_CMD)

clean:
	$(RM) $(OBJECTS)
	$(RM) $(TEST_OBJ)
	$(RM) $(TEST_TARGET)
	$(RM) *.txt
	$(RM) $(SHARED_LIB)
	$(RM) $(STATIC_LIB)

# Debug target
debug:
	@echo "Operating System: $(OS)"
	@echo "SHARED_LIB: $(SHARED_LIB)"
	@echo "SHARED_FLAGS: $(SHARED_FLAGS)"
	@echo "LDFLAGS: $(LDFLAGS)"
	@echo "SOURCES: $(SOURCES)"
	@echo "OBJECTS: $(OBJECTS)"
	@echo "Current directory: $(shell pwd)"