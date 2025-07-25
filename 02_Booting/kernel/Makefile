CROSS_COMPILE ?= aarch64-none-elf-
MODEL = raspi3b
CC := $(CROSS_COMPILE)gcc
LD := $(CROSS_COMPILE)ld
AR := $(CROSS_COMPILE)ar

COPS = -Wall -nostdlib -nostartfiles -ffreestanding -I../include -mgeneral-regs-only -g -O0
ASMOPS = -I../include -g -O0

MAKEFILE_DIR_ABS := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
BUILD_DIR = ../build/$(notdir $(MAKEFILE_DIR_ABS))
SRC_DIR = .

SRC_C_FILES := $(wildcard $(SRC_DIR)/*.c)
SRC_S_FILES := $(wildcard $(SRC_DIR)/*.S)

C_BASENAMES := $(notdir $(SRC_C_FILES:.c=))
S_BASENAMES := $(notdir $(SRC_S_FILES:.S=))

OBJ_FILES := $(addprefix $(BUILD_DIR)/, $(addsuffix _c.o, $(C_BASENAMES)))
OBJ_FILES += $(addprefix $(BUILD_DIR)/, $(addsuffix _s.o, $(S_BASENAMES)))

all: $(OBJ_FILES)

$(BUILD_DIR)/%_c.o: $(SRC_DIR)/%.c
	mkdir -p $(@D)
	$(CC) $(COPS) -MMD -c $< -o $@

$(BUILD_DIR)/%_s.o: $(SRC_DIR)/%.S
	mkdir -p $(@D)
	$(CC) $(ASMOPS) -MMD -c $< -o $@

DEP_FILES = $(OBJ_FILES:%.o=%.d)
-include $(DEP_FILES)

clean:
	rm -rf $(BUILD_DIR)