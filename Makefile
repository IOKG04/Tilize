# c compiler configuration
CC        := gcc
CVERSION  := -std=c11
CWARNS    := -Wall
CC_OPT    := -O0
CINCLUDES := -I/usr/include/SDL2 -D_REENTRANT
CLIBS     := -lm -L/usr/lib -lSDL2 -lSDL2_image
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

# cJSON
CJSON_C     := $(SRC_DIR)/cJSON.c
CJSON_C_URL := https://raw.githubusercontent.com/DaveGamble/cJSON/refs/heads/master/cJSON.c
ifeq ($(filter $(CJSON_C),$(SRCS)),)
	SRCS += $(CJSON_C)
	OBJS += $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(CJSON_C))
endif
CJSON_H     := $(SRC_DIR)/cJSON.h
CJSON_H_URL := https://raw.githubusercontent.com/DaveGamble/cJSON/refs/heads/master/cJSON.h
ifeq ($(filter $(CJSON_H),$(HEADERS)),)
	HEADERS += $(CJSON_H)
endif
$(CJSON_C):
	@mkdir -p $(dir $(CJSON_C))
	curl -s -o $@ $(CJSON_C_URL)
$(CJSON_H):
	@mkdir -p $(dir $(CJSON_H))
	curl -s -o $@ $(CJSON_H_URL)

# compile and link program
$(TARGET): $(OBJS)
	@mkdir -p $(dir $@)
	$(CC) -o $@ $^ $(CFLAGS)
$(OBJS): $(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS)
	@mkdir -p $(dir $@)
	$(CC) -o $@ $< -c $(CFLAGS)

# copy resources
$(RCS_OUTP): $(RCS_BIN_DIR)/%: $(RCS_DIR)/%
	@mkdir -p $(dir $@)
	cp $< $@

# downloads everything necessary for compiling
.PHONY: init
init: $(CJSON_H) $(CJSON_C)

# clean project build
.PHONY: clean
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)
.PHONY: cleanall
cleanall: clean
	rm -f $(CJSON_C) $(CJSON_H)
