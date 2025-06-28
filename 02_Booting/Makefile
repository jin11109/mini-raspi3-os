-include ../.env
export PATH := $(CROSS_COMPILER_PATH):$(PATH)

CROSS_COMPILE ?= aarch64-none-elf-
MODEL = raspi3b
LD := $(CROSS_COMPILE)ld
OBJCOPY := $(CROSS_COMPILE)objcopy

QEMUOPS = -cpu cortex-a53 -nographic -initrd initramfs.cpio -serial null -serial pty
BUILD_DIR = ./build

BOOTLOADER_OBJS = $(wildcard $(BUILD_DIR)/bootloader/*.o)
KERNEL_OBJS = $(wildcard $(BUILD_DIR)/kernel/*.o)
COMMAND_LIB := $(BUILD_DIR)/lib/libcommand.a

all: kernel8.img initramfs.cpio
kernel-pure: kernel8_pure.img initramfs.cpio

build-bootloader:
	$(MAKE) -C bootloader
build-kernel:
	$(MAKE) -C kernel
build-lib:
	$(MAKE) -C lib

kernel8.img: linker.ld build-bootloader build-kernel build-lib
	$(LD) -T linker.ld -o $(BUILD_DIR)/kernel8.elf --whole-archive $(COMMAND_LIB) $(KERNEL_OBJS) $(BOOTLOADER_OBJS)
	$(OBJCOPY) $(BUILD_DIR)/kernel8.elf -O binary kernel8.img

# Build the kernel without bootloader
kernel8_pure.img: ./kernel/linker.ld build-kernel build-lib
	$(LD) -T ./kernel/linker.ld -o $(BUILD_DIR)/kernel8_pure.elf --whole-archive $(COMMAND_LIB) $(KERNEL_OBJS)
	$(OBJCOPY) $(BUILD_DIR)/kernel8_pure.elf -O binary kernel8_pure.img

initramfs.cpio:
	cd rootfs && find . | cpio -o -H newc > ../initramfs.cpio

qemu: kernel8.img
	@qemu-system-aarch64 -M $(MODEL) -kernel kernel8.img $(QEMUOPS) 
qemu-debug: kernel8.img
	@qemu-system-aarch64 -M $(MODEL) -kernel kernel8.img $(QEMUOPS) -S -gdb tcp::1234

clean:
	rm -rf $(BUILD_DIR) *.img
	rm -f initramfs.cpio
	$(MAKE) -C bootloader clean
	$(MAKE) -C kernel clean