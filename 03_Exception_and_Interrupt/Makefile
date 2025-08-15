-include ../.env
export PATH := $(CROSS_COMPILER_PATH):$(PATH)

CROSS_COMPILE ?= aarch64-none-elf-
LD := $(CROSS_COMPILE)ld
OBJCOPY := $(CROSS_COMPILE)objcopy

BUILD_DIR = ./build

BOOTLOADER_OBJS = $(wildcard $(BUILD_DIR)/bootloader/*.o)
KERNEL_OBJS = $(wildcard $(BUILD_DIR)/kernel/*.o)
COMMAND_LIB := $(BUILD_DIR)/lib/libcommand.a

all: kernel8.img initramfs.cpio
kernel-pure: kernel8_pure.img initramfs.cpio

.PHONY: build-bootloader
build-bootloader:
	$(MAKE) -C bootloader
.PHONY: build-kernel
build-kernel:
	$(MAKE) -C kernel
.PHONY: build-lib
build-lib:
	$(MAKE) -C lib

kernel8.img: linker.ld build-bootloader build-kernel build-lib $(BUILD_DIR)/kernel8_pure_blob.o
	$(LD) -T linker.ld -Map=$(BUILD_DIR)/kernel8.map -o /dev/null $(BOOTLOADER_OBJS) $(COMMAND_LIB) $(BUILD_DIR)/kernel8_pure_blob.o
	$(LD) -T linker.ld -o $(BUILD_DIR)/kernel8.elf $(BOOTLOADER_OBJS) $(COMMAND_LIB) $(BUILD_DIR)/kernel8_pure_blob.o
	$(OBJCOPY) $(BUILD_DIR)/kernel8.elf -O binary kernel8.img

$(BUILD_DIR)/kernel8_pure_blob.o: kernel8_pure.img
	$(LD) -r -b binary -o $(BUILD_DIR)/kernel8_pure_blob.o kernel8_pure.img

# Build the kernel without bootloader
kernel8_pure.img: ./kernel/linker.ld build-kernel build-lib
	$(LD) -T ./kernel/linker.ld -o $(BUILD_DIR)/kernel8_pure.elf --whole-archive $(COMMAND_LIB) $(KERNEL_OBJS)
	$(OBJCOPY) $(BUILD_DIR)/kernel8_pure.elf -O binary kernel8_pure.img

initramfs.cpio:
	cd rootfs && find . | cpio -o -H newc > ../initramfs.cpio

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR) *.img
	rm -rf $(BUILD_DIR) *.o
	rm -f initramfs.cpio
	$(MAKE) -C bootloader clean
	$(MAKE) -C kernel clean