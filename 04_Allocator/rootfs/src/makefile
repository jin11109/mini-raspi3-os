CROSS_COMPILE ?= aarch64-none-elf-
MODEL = raspi3b
CC := $(CROSS_COMPILE)gcc
LD := $(CROSS_COMPILE)ld
OBJCOPY := $(CROSS_COMPILE)objcopy

COPS = -Wall -nostdlib -nostartfiles -ffreestanding -I../include -mgeneral-regs-only -O2
ASMOPS = -I../include -O2

BUILD_DIR = ../build/

all: $(BUILD_DIR) $(BUILD_DIR)test_exception.bin

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)test_exception.o: test_exception.S
	$(CC) $(ASMOPS) -MMD -c $< -o $@

$(BUILD_DIR)test_exception.elf: $(BUILD_DIR)test_exception.o linker.ld
	$(LD) -T linker.ld -o $@ $(BUILD_DIR)test_exception.o

$(BUILD_DIR)test_exception.bin: $(BUILD_DIR)test_exception.elf
	$(OBJCOPY) -O binary $< $(BUILD_DIR)test_exception.bin

clean:
	rm -rf $(BUILD_DIR)
