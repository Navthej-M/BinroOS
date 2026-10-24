// Copyright 2026 Navthej
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the “Software”), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

.section .data

# selector constants
.equ                GDT_KCODE, 0x08
.equ                GDT_KDATA, 0x10
.equ                GDT_UCODE, 0x1B
.equ                GDT_UDATA, 0x23
.equ                GDT_TSS, 0x28

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
    .word       103                                 # Limit: Size - 1 (104 - 1 = 103)
    .word       0x0000                              # Base (Bits 0-15)
    .byte       0x00                                # Base (Bits 16-23)
    .byte       0x89                                # Access: Present, Ring 0, Type: Available 64-bit TSS
    .byte       0x00                                # Flags and Limit (Bits 16-19)
    .byte       0x00                                # Base (Bits 24-31)
    .long       0x00000000                          # Base (Bits 32-63) -> Crucial: 32-bit field!
    .long       0x00000000                          # Reserved -> Crucial: Must be zero!
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

    movw            $GDT_TSS, %ax
    ltr             %ax

    movq            %rbp, %rsp
    popq            %rbp
    retq
