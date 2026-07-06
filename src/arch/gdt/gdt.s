.section .data

# selector constants
.equ                GDT_KCODE, 0x08
.equ                GDT_KDATA, 0x10
.equ                GDT_UCODE, 0x1B
.equ                GDT_UDATA, 0x23

.align 16
gdt_start:                                          # GDT STARTS HERE
    .quad           0x0000000000000000

gdt_kcode:                                          # Kernel Code
    .word           0xFFFF
    .word           0x0000
    .byte           0x00
    .byte           0x9A                            # Access: Present, Ring0, Code, Executable, Readable
    .byte           0xAF
    .byte           0x00

gdt_kdata:                                          # Kernel Data
    .word           0xFFFF
    .word           0x0000
    .byte           0x00
    .byte           0x92                            # Access: Present, Ring0, Data, Writable
    .byte           0xCF
    .byte           0x00

gdt_ucode:                                          # User Code
    .word           0xFFFF
    .word           0x0000
    .byte           0x00
    .byte           0xFA                            # Access: Present, Ring3, Code, Executable, Readable
    .byte           0xAF
    .byte           0x00

gdt_udata:                                          # User Data
    .word           0xFFFF
    .word           0x0000
    .byte           0x00
    .byte           0xF2                            # Access: Present, Ring3, Data, Writable
    .byte           0xCF
    .byte           0x00

tss_start:
.global gdt_tss
gdt_tss:
    .word       104                                 # Limit: A 64-bit TSS is exactly 104 bytes long
    .word       0x0000                              # Base (Bits 0-15) -> Zeroed
    .byte       0x00                                # Base (Bits 16-23) -> Zeroed
    .byte       0x89                                # Access: Present, Ring 0, Type: Available 64-bit TSS
    .byte       0x00                                # Flags and Limit (Bits 16-19) -> Zeroed
    .byte       0x00                                # Base (Bits 24-31) -> Zeroed
    .quad       0x0000000000000000                  # Base (Bits 32-63) -> Zeroed
tss_end:

gdt_end:                                            # GDT ENDS HERE
gdt_descriptor:
    .word           gdt_end - gdt_start - 1
    .quad           gdt_start

.section .text

#
# gdt_load (defined in include/kernel/gdt.h)
#
# This function loads the GDT.
#
.global gdt_load
.type gdt_load, @function
gdt_load:
    pushq           %rbp
    movq            %rsp, %rbp

    lgdt            gdt_descriptor(%rip)

    pushq           $GDT_KCODE
    leaq            .gdt_reload(%rip), %rax
    pushq           %rax
    lretq

.gdt_reload:
    movw            $GDT_KDATA, %ax
    movw            %ax, %ds
    movw            %ax, %es
    movw            %ax, %fs
    movw            %ax, %gs
    movw            %ax, %ss

    movq            %rbp, %rsp
    popq            %rbp
    retq
