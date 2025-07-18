# GRIDx64
A 64-bit hobby operating system

# TODO
- [] Explicitly create the GDT and configure paging
- [] Initialize a serial port, if available, for debugging
- [] Make console driver more robust to framebuffer detecting
- [] Detect and store ACPI tables alongside other system information
- [] Create a heap and dynamic memory allocation techniques
- [] Add macros to customize compilation for different firmwares and architectures
- [] Create a function that will decode error codes from CPU exceptions and display
     them in a human readable format
- [] YOU HAVE TO FIX CONSLE_PRINTF TO ACCOUNT FOR UNSIGNED INTEGERS!!!
- [] YOU NEED TO FIX RIND_RSDP() BECAUSE YOU'RE DEFAULTING TO ACPI 1.0!!!

# NOTES
- Grub 2 with Multiboot 2 will likely fail to boot if you don't align your tags to 8 bytes.
- In protected mode, you have to be exceedingly careful to add the right amount of bytes to
  the stack after your main ISR returns to the stub that called it. This is because of the
  vector and error code pushed.
- If your kernel is failing, make sure to use GDB and manually inspect the stack and disassembly
  of the routine in question, to verify that the right values are being passed.
- Interrupts may fail if you don't properly reroute the 8259 PIC, mainly to vectors 32-47.
- For GRUB 2, you MUST add "insmod efi_gop" to your "grub.cfg" file, or else you might
  get a message saying "error: no suitable video mode found".
- To boot in Long Mode, you need to define both the "EFI amd64 entry address" and
  "EFI boot services" tags, specifying the entry point of your OS (usually _start) as the load
  address in the first of the two tags.
- You may run "sudo dmesg | grep -i 8042" to see if your computer has a legacy keyboard controller
- 