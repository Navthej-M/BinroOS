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
/// @brief Helper functions to manage CPU states like hang, interrupts on off,
/// and more like that.

#ifndef _IRQ_H
#define _IRQ_H

#include "libk/types.h"

// will get returned by arch_interrupts()
static bool _interupts = false;

/// @brief Will disable interupts
static inline void arch_disable_interupts(void) {
    __asm__ volatile("cli");
    _interupts = false;
}

/// @brief Will enable interupts
static inline void arch_enable_interupts(void) {
    __asm__ volatile("sti");
    _interupts = true;
}

/// @brief Will halt the CPU
static inline void arch_halt(void) { __asm__ volatile("hlt"); }

/// @brief Will hang the CPU by clering intrupts and halting forever.
static inline void arch_hang(bool disable_interupts) {
    if (disable_interupts) {
        arch_disable_interupts();
    }
    for (;;) {
        arch_halt();
    }
}

/// @brief will return true if intrupts are on and vice-versa.
///
/// @return If interupts are ON or OFF rn.
///
/// @note ONLY if interrupts are turned on/off using arch_enable_interrupts()
/// and arch_disable_interrupts().
static inline bool arch_interrupts(void) { return _interupts; }

#endif
