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

#include "arch/tss.h"
#include "libk/core.h"

static tss_entry_t g_tss;

void tss_init(uint64_t kernel_stack_top) {
    uint8_t *tss_ptr = (uint8_t *)&g_tss;
    libk_memset(tss_ptr, 0, sizeof(tss_entry_t));

    g_tss.rsp0 = kernel_stack_top;

    // since the CPU understands that the iomap_base is completely disabled for
    // user if iomap_base >= TSS_LIMIT, setting it exactly to the size of the
    // struct
    g_tss.iomap_base = (uint16_t)sizeof(tss_entry_t);

    uint64_t base = (uint64_t)&g_tss;

    gdt_tss[2] = (uint8_t)(base & 0xFF);         // Base Bits 0-7
    gdt_tss[3] = (uint8_t)((base >> 8) & 0xFF);  // Base Bits 8-15
    gdt_tss[4] = (uint8_t)((base >> 16) & 0xFF); // Base Bits 16-23
    gdt_tss[7] = (uint8_t)((base >> 24) & 0xFF); // Base Bits 24-31

    // handling the high 32 bits
    uint32_t base_high = (uint32_t)(base >> 32);

    gdt_tss[8] = (uint8_t)(base_high & 0xFF);
    gdt_tss[9] = (uint8_t)((base_high >> 8) & 0xFF);
    gdt_tss[10] = (uint8_t)((base_high >> 16) & 0xFF);
    gdt_tss[11] = (uint8_t)((base_high >> 24) & 0xFF);

    // NOTE: The remaining ([12]-[16]) are reserved to the x86 CPU
}
