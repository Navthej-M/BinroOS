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

/// @file
/// @brief Contains required structs and functions for TSS

#ifndef _tss_h
#define _tss_h

#include "libk/types.h"

/// @brief struct for a tss enyry
typedef struct __attribute__((packed)) {
    uint32_t reserved0;
    uint64_t rsp0; // Stack pointer loaded on Ring 3 -> Ring 0 switch
    uint64_t rsp1; // Stack pointer loaded on Ring 3 -> Ring 1 switch
    uint64_t rsp2; // Stack pointer loaded on Ring 3 -> Ring 2 switch
    uint64_t reserved1;
    uint64_t ist[7]; // Interrupt Stack Table (IST 1 to 7)
    uint64_t reserved2;
    uint16_t reserved3;
    uint16_t
        iomap_base; // Offset to I/O Permission Bitmap (usually sizeof(struct))
} tss_entry_t;

/// @brief gdt_tss array symbol exists inside the assembly (src/arch/gdt.s)
extern uint8_t gdt_tss[];

/// @brief Initilize TSS
///
/// @param kernel_stack_top the sack address of the kernel
void tss_init(uint64_t kernel_stack_top);

#endif
