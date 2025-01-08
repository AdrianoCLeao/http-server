CC = gcc
CFLAGS = -Wall -I$(INCLUDE_DIR)
SRC_DIR = src
BUILD_DIR = bin
INCLUDE_DIR = include

SRCS := $(wildcard $(SRC_DIR)/**/*.c) $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))
TARGET := $(BUILD_DIR)/main.exe

ifeq ($(OS),Windows_NT)
    LDFLAGS = -lws2_32
    MKDIR_P = if not exist "$(subst /,\,$(strip $(1)))" mkdir "$(subst /,\,$(strip $(1)))"
    RMDIR = rmdir /s /q
    EXECUTABLE = $(TARGET)
else
    LDFLAGS =
    MKDIR_P = mkdir -p $(1)
    RMDIR = rm -rf
    EXECUTABLE = ./$(TARGET)
endif

all: $(TARGET)

$(TARGET): $(OBJS)
	@$(call MKDIR_P, $(BUILD_DIR))
	@$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@$(call MKDIR_P, $(dir $@))
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@$(RMDIR) $(BUILD_DIR)

run: $(TARGET)
	@$(EXECUTABLE)

.PHONY: all clean run