
AS := x86_64-elf-gcc
CC := x86_64-elf-gcc
LD := x86_64-elf-gcc

SRC_DIR   := src
BUILD_DIR := build
OBJ_DIR   := $(BUILD_DIR)/obj
BIN_DIR   := $(BUILD_DIR)/bin

ISO_DIR := iso
ISO_IMAGE := $(BIN_DIR)/GRIDx64.iso

INCLUDE_DIRS := include

LINKER_SCRIPT := linker.ld
KERNEL_ELF    := $(BIN_DIR)/kernel.elf

ASFLAGS := -ffreestanding -nostdlib -I$(INCLUDE_DIRS) -Isrc -g -O0
CFLAGS  := -ffreestanding -nostdlib -O2 -Wall -Wextra -I$(INCLUDE_DIRS) -Isrc -g -O0
LDFLAGS := -ffreestanding -nostdlib -T$(LINKER_SCRIPT) -g -O0

ASM_SOURCES := $(shell find $(SRC_DIR) -name '*.S')
C_SOURCES   := $(shell find $(SRC_DIR) -name '*.c')

ASM_OBJECTS := $(patsubst $(SRC_DIR)/%.S, $(OBJ_DIR)/%.o, $(ASM_SOURCES))
C_OBJECTS   := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(C_SOURCES))

OBJECTS := $(ASM_OBJECTS) $(C_OBJECTS)

all: $(KERNEL_ELF) $(ISO_IMAGE)

run: $(ISO_IMAGE)
	qemu-system-x86_64 -cdrom $(ISO_IMAGE) -m 4G -smp 10 -bios /usr/share/ovmf/OVMF.fd -serial stdio

install: $(ISO_IMAGE)
	sudo dd if=$(ISO_IMAGE) of=/dev/sdb bs=512 conv=notrunc && sync

$(ISO_IMAGE): $(KERNEL_ELF)
	cp $(KERNEL_ELF) $(ISO_DIR)/boot/kernel.elf
	grub-mkrescue $(ISO_DIR) -o $(ISO_IMAGE)

$(KERNEL_ELF): $(OBJECTS) | $(BIN_DIR)
	$(LD) $(LDFLAGS) $^ -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.S | $(OBJ_DIR)
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) -I$(dir $<) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I$(dir $<) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

clean:
	rm -rf $(BUILD_DIR) $(ISO_DIR)/boot/kernel.elf

.PHONY: all clean



