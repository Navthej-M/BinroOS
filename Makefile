#
# Environment & Tools
#
export ROOT_DIR       := $(shell pwd)
export INCLUDE_DIR    := $(ROOT_DIR)/include
export C_OPTIMIZATION := -Og

CC        := gcc
LD        := ld
AS        := gcc
EMU_QEMU  := qemu-system-x86_64
EMU_BOCHS := bochs

#
# Configuration & Paths
#
LIMINE_DIR ?= /usr/share/limine

CONFIG_DIR  := config
BUILD_DIR   := build
IMG_DIR     := img
ISO_ROOT    := $(BUILD_DIR)/iso_root
LIBS        := libs

KERNEL_ELF  := $(BUILD_DIR)/kernel.elf
TARGET_ISO  := $(IMG_DIR)/binro.iso
DRIVER_LIB  := $(LIBS)/drivers.a

LIMINE_CONF := $(CONFIG_DIR)/limine/limine.conf
BOCHS_CONF  := $(CONFIG_DIR)/bochs/bochsrc.ini

#
# Debug
#
# Uncomment the next lines to set debug true accordingly.
#
# Uncomming the next line to enable debugging for C and Asm code.
# DEBUG = -g
#
# Uncomment the next line to enable GDB debugging.
# QEMU_DEBUG = -s -S
#
# Uncomment the next line to enable Bochs debugging.
# BOCHS_DEBUG = dbg

#
# Smart Source Resolution
#
# Recursively find all .c and .s files anywhere inside src/,
# BUT explicitly ignore the src/drivers/ directory (it has its own Makefile).
C_SRCS     := $(shell find src -type d -name drivers -prune -o -type f -name '*.c' -print)
ASM_SRCS   := $(shell find src -type d -name drivers -prune -o -type f -name '*.s' -print)

# Translate paths: src/kernel/idt/idt.c -> build/src/kernel/idt/idt.c.o
C_OBJS     := $(patsubst %.c, $(BUILD_DIR)/%.c.o, $(C_SRCS))
ASM_OBJS   := $(patsubst %.s, $(BUILD_DIR)/%.s.o, $(ASM_SRCS))
KERNEL_OBJS := $(C_OBJS) $(ASM_OBJS)

# Generate dependency files (.d)
DEPS       := $(C_OBJS:.o=.d)

#
# Compiler & Linker Flags
#
CFLAGS := -I$(INCLUDE_DIR) $(C_OPTIMIZATION) -std=c23 -m64 -march=x86-64 -fno-pic -fno-pie \
          -ffreestanding -fno-stack-protector -fno-stack-check \
          -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -mcmodel=kernel \
          -Wall -Wextra -MMD -MP

LDFLAGS := -m elf_x86_64 -static -nostdlib -no-pie -T linker.ld

QEMU_FLAGS  := $(QEMU_DEBUG) -display gtk,zoom-to-fit=on,full-screen=on -cdrom $(TARGET_ISO)
BOCHS_FLAGS := -f $(BOCHS_CONF) -q $(BOCHS_DEBUG)

AS_FLAGS := -Wall -Wextra -ffreestanding -c

#
# Build Rules
#
.PHONY: all clean emulate drivers

all: $(TARGET_ISO)

# Compile C files automatically
$(BUILD_DIR)/%.c.o: %.c
	@mkdir -p $(@D)
	$(CC) $(DEBUG) $(CFLAGS) -c $< -o $@

# Compile ASM files automatically
$(BUILD_DIR)/%.s.o: %.s
	@mkdir -p $(@D)
	$(AS) $(DEBUG) $(AS_FLAGS) $< -o $@

# Build driver library via sub-make
drivers:
	@mkdir -p $(LIBS)
	$(MAKE) -C src/drivers/

# Link the final Kernel ELF
$(KERNEL_ELF): $(KERNEL_OBJS) drivers
	@mkdir -p $(@D)
	$(LD) $(LDFLAGS) $(KERNEL_OBJS) $(DRIVER_LIB) -o $@

# Package into bootable ISO
$(TARGET_ISO): $(KERNEL_ELF) $(LIMINE_CONF)
	@mkdir -p $(ISO_ROOT)/boot $(IMG_DIR)
	
	cp $(KERNEL_ELF) $(ISO_ROOT)/boot/kernel.elf
	cp $(LIMINE_CONF) $(ISO_ROOT)/boot/
	
	cp $(LIMINE_DIR)/limine-bios.sys $(ISO_ROOT)/boot/
	cp $(LIMINE_DIR)/limine-bios-cd.bin $(ISO_ROOT)/boot/
	cp $(LIMINE_DIR)/limine-uefi-cd.bin $(ISO_ROOT)/boot/

	xorriso -as mkisofs -b boot/limine-bios-cd.bin \
		-no-emul-boot -boot-load-size 4 -boot-info-table \
		--efi-boot boot/limine-uefi-cd.bin \
		-efi-boot-part --efi-boot-image --protective-msdos-label \
		$(ISO_ROOT) -o $(TARGET_ISO)

	limine bios-install $(TARGET_ISO)

emulate-qemu: $(TARGET_ISO)
	$(EMU_QEMU) $(QEMU_FLAGS)

emulate-bochs:
	$(EMU_BOCHS) $(BOCHS_FLAGS)

clean:
	rm -rf $(BUILD_DIR) $(IMG_DIR) $(LIBS)
	$(MAKE) -C src/drivers clean

-include $(DEPS)
