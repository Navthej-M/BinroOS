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
/// @brief Functions to manage GDT.
///
/// GDT is fully written in Assembly (src/arch/gdt.s). This header only
/// contains macros to specific address for simplicity and a function to
/// initilize GDT (which is written in asm btw) The reason why I did it might be
/// a lil bit wiered... assembly feels more easier in that case for me.
///
/// Before using Limine, I tried to make my own bootloader for BIOS finally to
/// realize that it itself only would take months to make a good one and I would
/// be then learning 90s computer archititure insted of "os dev". So, I gave it
/// up. However, in middle of that journey, where I have to start from 16 bit
/// real mode, I got soo used with making GDT in Assembly. So, I thought, let's
/// just do that again, and I did it.

#ifndef _GDT_H
#define _GDT_H

// Selector constants
#define GDT_KCODE 0x08
#define GDT_KDATA 0x10
#define GDT_UCODE 0x1B
#define GDT_UDATA 0x23
#define GDT_TSS 0x28

/// @brief Will load the GDT
extern void gdt_load(void);

#endif
