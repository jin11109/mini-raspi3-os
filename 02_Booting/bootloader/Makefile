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

OBJ_FILES := $(SRC_C_FILES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%_c.o)
OBJ_FILES += $(SRC_S_FILES:$(SRC_DIR)/%.S=$(BUILD_DIR)/%_s.o)

$(BUILD_DIR)/%_c.o: $(SRC_DIR)/%.c
	mkdir -p $(@D)
	$(CC) $(COPS) -MMD -c $< -o $@

$(BUILD_DIR)/%_s.o: $(SRC_DIR)/%.S
	mkdir -p $(@D)
	$(CC) $(ASMOPS) -MMD -c $< -o $@

DEP_FILES = $(OBJ_FILES:%.o=%.d)
-include $(DEP_FILES)

all: $(OBJ_FILES)

clean:
	rm -rf $(BUILD_DIR)

# CROSS_COMPILE ?= aarch64-none-elf-
# MODEL = raspi3b
# CC := $(CROSS_COMPILE)gcc
# LD := $(CROSS_COMPILE)ld
# AR := $(CROSS_COMPILE)ar

# COPS = -Wall -nostdlib -nostartfiles -ffreestanding -I../include -mgeneral-regs-only -g -O0
# ASMOPS = -I../include -g -O0

# MAKEFILE_DIR_ABS := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
# BUILD_DIR = ../build/$(notdir $(MAKEFILE_DIR_ABS))
# SRC_DIR = .
# LIBSRCS_DIR := ../lib

# SRC_C_FILES := $(wildcard $(SRC_DIR)/*.c)
# LIB_C_FILES := $(wildcard $(LIBSRCS_DIR)/*.c)

# SRC_S_FILES := $(wildcard $(SRC_DIR)/*.S)
# LIB_S_FILES := $(wildcard $(LIBSRCS_DIR)/*.S)

# OBJ_FILES := $(SRC_C_FILES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%_c.o)
# OBJ_FILES += $(LIB_C_FILES:$(LIBSRCS_DIR)/%.c=$(BUILD_DIR)/lib/%_c.o)

# OBJ_FILES += $(SRC_S_FILES:$(SRC_DIR)/%.S=$(BUILD_DIR)/%_s.o)
# OBJ_FILES += $(LIB_S_FILES:$(LIBSRCS_DIR)/%.S=$(BUILD_DIR)/lib/%_s.o)

# $(BUILD_DIR)/%_c.o: $(SRC_DIR)/%.c
# 	mkdir -p $(@D)
# 	$(CC) $(COPS) -MMD -c $< -o $@

# $(BUILD_DIR)/lib/%_c.o: $(LIBSRCS_DIR)/%.c
# 	mkdir -p $(@D)
# 	$(CC) $(COPS) -MMD -c $< -o $@

# $(BUILD_DIR)/%_s.o: $(SRC_DIR)/%.S
# 	mkdir -p $(@D)
# 	$(CC) $(ASMOPS) -MMD -c $< -o $@

# $(BUILD_DIR)/lib/%_s.o: $(LIBSRCS_DIR)/%.S
# 	mkdir -p $(@D)
# 	$(CC) $(ASMOPS) -MMD -c $< -o $@

# DEP_FILES = $(OBJ_FILES:%.o=%.d)
# -include $(DEP_FILES)

# $(BUILD_DIR)/libbootloader.a: $(OBJ_FILES)
# 	$(AR) rcs $@ $^

# all: $(BUILD_DIR)/libbootloader.a

# clean:
# 	rm -rf $(BUILD_DIR)