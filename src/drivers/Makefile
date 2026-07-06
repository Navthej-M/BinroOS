#
# Tools & Environment
#
# Fallbacks just in case this is run directly instead of via the root Makefile
ROOT_DIR    ?= $(abspath ../..)
INCLUDE_DIR ?= $(ROOT_DIR)/include

CC      := gcc
AS      := gcc
AR      := ar

#
# Configuration & Paths
#
CENTRAL_BUILD_DIR := $(ROOT_DIR)/build/drivers
LIBS              := $(ROOT_DIR)/libs
TARGET            := $(LIBS)/drivers.a

#
# Compiler & Linker Flags (Strictly aligned with Root Makefile)
#
# Drivers MUST share the same architecture, code model, and freestanding flags
CFLAGS := -I$(INCLUDE_DIR) $(C_OPTIMIZATION) -m64 -march=x86-64 -fno-pic -fno-pie \
          -ffreestanding -fno-stack-protector -fno-stack-check \
          -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -mcmodel=kernel \
          -Wall -Wextra -MMD -MP

ASFLAGS := -Wall -Wextra -ffreestanding -m64 -march=x86-64 -c

#
# Smart Source Resolution
#
# Find all .c and .s files, stripping the leading './' to keep paths clean
C_SRCS  := $(patsubst ./%, %, $(shell find . -type f -name "*.c"))
S_SRCS  := $(patsubst ./%, %, $(shell find . -type f -name "*.s"))

# Map to .c.o and .s.o to match the root Makefile conventions exactly
C_OBJS  := $(patsubst %.c, $(CENTRAL_BUILD_DIR)/%.c.o, $(C_SRCS))
S_OBJS  := $(patsubst %.s, $(CENTRAL_BUILD_DIR)/%.s.o, $(S_SRCS))
OBJS    := $(C_OBJS) $(S_OBJS)

# Generate dependency files (.d) so drivers rebuild when headers change
DEPS    := $(OBJS:.o=.d)

#
# Build Rules
#
.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(@D)
	$(AR) rcs $@ $^

# Rule for compiling C files
$(CENTRAL_BUILD_DIR)/%.c.o: %.c
	@mkdir -p $(@D)
	$(CC) $(DEBUG) $(CFLAGS) -c $< -o $@

# Rule for assembling Assembly (.s) files
$(CENTRAL_BUILD_DIR)/%.s.o: %.s
	@mkdir -p $(@D)
	$(AS) $(DEBUG) $(ASFLAGS) $< -o $@

clean:
	rm -rf $(CENTRAL_BUILD_DIR) $(TARGET)

-include $(DEPS)
