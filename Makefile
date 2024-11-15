# c compiler configuration
CC        := gcc
CVERSION  := -std=gnu11
CWARNS    := -Wall
CC_OPT    := -O0
CINCLUDES :=
CLIBS     :=
CDEFINES  :=

# source, input resources, object and binary directories
SRC_DIR := src
RCS_DIR := resources
OBJ_DIR := obj
BIN_DIR := bin

# target name
TARGET_NAME := Tilize

# ###
# CONFIGURATION END

# combined cflags
CFLAGS := $(CVERSION) $(CWARNS) $(CC_OPT) $(CINCLUDES) $(CLIBS) $(CDEFINES)

# target and output resources position
TARGET      := $(BIN_DIR)/$(TARGET_NAME)
RCS_BIN_DIR := $(BIN_DIR)/$(RCS_DIR)

# get source, object and header files
SRCS    := $(shell find $(SRC_DIR)/ -name "*.c")
OBJS    := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))
HEADERS := $(shell find $(SRC_DIR)/ -name "*.h")

# get input and output resource files
RCS_INP  := $(shell find $(RCS_DIR)/ -type f)
RCS_OUTP := $(patsubst $(RCS_DIR)/%,$(RCS_BIN_DIR)/%,$(RCS_INP))

# completely build project
.PHONY: all
all: $(TARGET) $(RCS_OUTP)

# compile and link program
$(TARGET): $(OBJS)
	@mkdir -p $(dir $@)
	$(CC) -o $@ $^ $(CFALGS)
$(OBJS): $(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS)
	@mkdir -p $(dir $@)
	$(CC) -o $@ $< -c $(CFLAGS)

# copy resources
$(RCS_OUTP): $(RCS_BIN_DIR)/%: $(RCS_DIR)/%
	@mkdir -p $(dir $@)
	cp $< $@

# clean project build
.PHONY: clean
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)
