SRC_DIR   := src
BUILD_DIR := build
OBJ_DIR   := $(BUILD_DIR)/obj
BIN_DIR   := $(BUILD_DIR)/bin
LD_SCRIPT := linker.ld

KERNEL := $(BIN_DIR)/kernel.elf

EFI_BOOT := /home/ondine/src/edk2/Grid/build/Grid.efi
EFI_IMG  := $(BUILD_DIR)/uefi.img

CC     := /home/ondine/opt/cross/bin/x86_64-elf-gcc
CFLAGS := -ffreestanding -nostdlib -Isrc -Iinclude -O2 -mno-red-zone -mcmodel=large -g

ASM_SOURCES := $(shell find $(SRC_DIR) -name '*.S')
C_SOURCES   := $(shell find $(SRC_DIR) -name '*.c')

ASM_OBJECTS := $(patsubst $(SRC_DIR)/%.S, $(OBJ_DIR)/%.o, $(ASM_SOURCES))
C_OBJECTS   := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(C_SOURCES))
OBJECTS     := $(ASM_OBJECTS) $(C_OBJECTS)

.PHONY: all clean run

all: $(EFI_IMG)

run: $(EFI_IMG)
	qemu-system-x86_64 -m 16G -smp 10 -drive file=$<,format=raw -bios /usr/share/ovmf/OVMF.fd

$(EFI_IMG): $(EFI_BOOT) $(KERNEL)
	@echo "  IMG     $@"
	@sudo dd if=/dev/zero of=$@ bs=1M count=64 status=none
	@sudo mformat -i $@ -F -v EFIIMG ::
	@sudo mmd -i $@ ::/EFI
	@sudo mmd -i $@ ::/EFI/BOOT
	@sudo mcopy -i $@ $(EFI_BOOT) ::/EFI/BOOT/BOOTX64.EFI
	@sudo mcopy -i $@ $(KERNEL) ::/kernel.elf


$(OBJ_DIR)/%.o: $(SRC_DIR)/%.S
	@mkdir -p $(dir $@)
	@echo "  AS      $<"
	@$(CC) $(ASFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo "  CC      $<"
	@$(CC) $(CFLAGS) -c $< -o $@

$(BIN_DIR):
	@mkdir -p $@

$(KERNEL): $(OBJECTS) | $(BIN_DIR)
	@echo "  LD      $@"
	@$(CC) $(CFLAGS) -T $(LD_SCRIPT) $^ -o $@

clean:
	@echo "Cleaning..."
	@rm -rf $(BUILD_DIR)
