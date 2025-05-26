CROSS_COMPILE ?= aarch64-none-elf-
MODEL = raspi3b
CC := $(CROSS_COMPILE)gcc

COPS = -Wall -nostdlib -nostartfiles -ffreestanding -Iinclude -mgeneral-regs-only -g -O0
ASMOPS = -Iinclude -g -O0
QEMUOPS = -nographic -serial null -serial mon:stdio

BUILD_DIR = build
SRC_DIR = src

all: kernel8.img

qemu: kernel8.img
	@qemu-system-aarch64 -M $(MODEL) -kernel kernel8.img $(QEMUOPS) 
qemu-debug: clean kernel8.img
	@qemu-system-aarch64 -M $(MODEL) -kernel kernel8.img $(QEMUOPS) -S -gdb tcp::1234

clean:
	rm -rf $(BUILD_DIR) *.img 

$(BUILD_DIR)/%_c.o: $(SRC_DIR)/%.c
	mkdir -p $(@D)
	$(CC) $(COPS) -MMD -c $< -o $@

$(BUILD_DIR)/%_s.o: $(SRC_DIR)/%.S
	$(CC) $(ASMOPS) -MMD -c $< -o $@

C_FILES = $(wildcard $(SRC_DIR)/*.c)
ASM_FILES = $(wildcard $(SRC_DIR)/*.S)
OBJ_FILES = $(C_FILES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%_c.o)
OBJ_FILES += $(ASM_FILES:$(SRC_DIR)/%.S=$(BUILD_DIR)/%_s.o)

DEP_FILES = $(OBJ_FILES:%.o=%.d)
-include $(DEP_FILES)

kernel8.img: $(SRC_DIR)/linker.ld $(OBJ_FILES)
	$(CROSS_COMPILE)ld -T $(SRC_DIR)/linker.ld -o $(BUILD_DIR)/kernel8.elf  $(OBJ_FILES)
	$(CROSS_COMPILE)objcopy $(BUILD_DIR)/kernel8.elf -O binary kernel8.img
