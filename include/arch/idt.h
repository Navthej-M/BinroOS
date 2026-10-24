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
/// @brief Functions to manage IDT and Interrupts.

#ifndef _IDT_H
#define _IDT_H

#include "libk/types.h"

#define IDT_ENTRIES 256

/// @brief This struct stores one entry for IDT.
typedef struct __attribute__((packed)) {
    uint16_t base_low;  ///< Lower 16 bits of handler address.
    uint16_t selector;  ///< Codes segment
    uint8_t ist;        ///< Interrupt stack table
    uint8_t type_attr;  ///< Type and attributes
    uint16_t base_mid;  ///< Middle 16 bits of handler address
    uint32_t base_high; ///< Upper 32 bits of handler address
    uint32_t reserved;  ///< Reserved space; must be zero.
} idt_entry;

/// @brief A pointer for 'idt_entry'
typedef struct __attribute__((packed)) {
    uint16_t limit; ///< size of IDT - 1
    uint64_t base;  ///< Address of IDT
} idt_ptr;

/// @brief contains the required information of specific interrupts.
typedef struct __attribute__((packed)) {
    /// @name General purpose registers
    /// @{
    uint64_t r15;
    uint64_t r14;
    uint64_t r13;
    uint64_t r12;
    uint64_t r11;
    uint64_t r10;
    uint64_t r9;
    uint64_t r8;
    uint64_t rbp;
    uint64_t rdi;
    uint64_t rsi;
    uint64_t rdx;
    uint64_t rcx;
    uint64_t rbx;
    uint64_t rax;
    /// @}

    /// @name Handler details
    /// @{
    uint64_t vector;
    uint64_t error_code;
    /// @}

    /// @name Automatically pushed by the CPU
    /// @{
    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
    uint64_t rsp;
    uint64_t ss;
    /// @}
} interrupt_frames;

/// @brief Will initilize IDT.
void idt_init(void);

/// @brief Creates an entry in the Interrupt Descriptor Table (IDT).
///
/// @param vector The interrupt vector number (0-255).
/// @param isr_handler The assembly ISR stub (saves registers, calls the
///                    actual handler, and then restores registers).
/// @param handler The actual C/C++ handler function.
/// @param type_attr The type and attribute byte (e.g., privilege level, gate
///                  type).
/// @param ist The Interrupt Stack Table index to use (0 to use the current
///            stack).
void idt_set_entry(uint8_t vector, void *isr_handler,
                   void (*handler)(interrupt_frames *), uint8_t type_attr,
                   uint8_t ist);

/// @bried Will set intrrupt handler function for the specific vector.
///
/// @param vector The vector of the intrrupt
/// @param handler The handler function for that specific intrrupt
void idt_set_handler_function(uint8_t vector,
                              void (*handler)(interrupt_frames *));

// =============================================================================
// ISR LOW-LEVEL ASSEMBLY STUBS
// =============================================================================
// These are the direct targets for the IDT. They handle saving/restoring
// registers and pushing dummy error codes where necessary before calling the
// C handlers.

/// @brief ISR Stub for Vector 0: Divide-by-Zero Error (#DE).
/// @note No error code pushed by CPU.
void isr0(void);

/// @brief ISR Stub for Vector 1: Debug Exception (#DB).
/// @note No error code pushed by CPU.
void isr1(void);

/// @brief ISR Stub for Vector 2: Non-Maskable Interrupt (#NMI).
/// @note No error code pushed by CPU.
void isr2(void);

/// @brief ISR Stub for Vector 3: Breakpoint Exception (#BP).
/// @note No error code pushed by CPU.
void isr3(void);

/// @brief ISR Stub for Vector 4: Overflow Exception (#OF).
/// @note No error code pushed by CPU.
void isr4(void);

/// @brief ISR Stub for Vector 5: BOUND Range Exceeded Exception (#BR).
/// @note No error code pushed by CPU.
void isr5(void);

/// @brief ISR Stub for Vector 6: Invalid Opcode Exception (#UD).
/// @note No error code pushed by CPU.
void isr6(void);

/// @brief ISR Stub for Vector 7: Device Not Available Exception (#NM).
/// @note No error code pushed by CPU.
void isr7(void);

/// @brief ISR Stub for Vector 8: Double Fault Exception (#DF).
/// @note CPU pushes an error code.
void isr8(void);

/// @brief ISR Stub for Vector 10: Invalid TSS Exception (#TS).
/// @note CPU pushes an error code.
void isr10(void);

/// @brief ISR Stub for Vector 11: Segment Not Present (#NP).
/// @note CPU pushes an error code.
void isr11(void);

/// @brief ISR Stub for Vector 12: Stack-Segment Fault (#SS).
/// @note CPU pushes an error code.
void isr12(void);

/// @brief ISR Stub for Vector 13: General Protection Fault (#GP).
/// @note CPU pushes an error code.
void isr13(void);

/// @brief ISR Stub for Vector 14: Page Fault Exception (#PF).
/// @note CPU pushes an error code.
void isr14(void);

/// @brief ISR Stub for Vector 16: x87 FPU Floating-Point Error (#MF).
/// @note No error code pushed by CPU.
void isr16(void);

/// @brief ISR Stub for Vector 17: Alignment Check Exception (#AC).
/// @note CPU pushes an error code.
void isr17(void);

/// @brief ISR Stub for Vector 18: Machine Check Exception (#MC).
/// @note No error code pushed by CPU.
void isr18(void);

/// @brief ISR Stub for Vector 19: SIMD Floating-Point Exception (#XF).
/// @note No error code pushed by CPU.
void isr19(void);

/// @brief ISR Stub for Vector 20: Virtualization Exception (#VE).
/// @note No error code pushed by CPU.
void isr20(void);

/// @brief ISR Stub for Vector 21: Control Protection Exception (#CP).
/// @note CPU pushes an error code.
void isr21(void);

/// @brief ISR Stub for Vector 28: Hypervisor Injection Exception (#HV).
/// @note No error code pushed by CPU.
void isr28(void);

/// @brief ISR Stub for Vector 29: VMM Communication Exception (#VC).
/// @note CPU pushes an error code.
void isr29(void);

/// @brief ISR Stub for Vector 30: Security Exception (#SX).
/// @note CPU pushes an error code.
void isr30(void);

/// @brief ISR Stub for Vector 33: IRQ1 - Keyboard Interrupt.
/// @note No error code pushed by CPU.
void isr33(void);

// =============================================================================
// ISR HIGH-LEVEL C HANDLERS
// =============================================================================
// Called by the assembly stubs after the register state has been safely saved
// onto the stack.

/// @brief High-level C handler for Divide-by-Zero Error (#DE).
/// @param intrrupt Pointer to the saved processor state.
void isr0_handler(interrupt_frames *intrrupt);

/// @brief High-level C handler for Debug Exception (#DB).
/// @param intrrupt Pointer to the saved processor state.
void isr1_handler(interrupt_frames *intrrupt);

/// @brief High-level C handler for Non-Maskable Interrupt (#NMI).
/// @param intrrupt Pointer to the saved processor state.
void isr2_handler(interrupt_frames *intrrupt);

/// @brief High-level C handler for Breakpoint Exception (#BP).
/// @param intrrupt Pointer to the saved processor state.
void isr3_handler(interrupt_frames *intrrupt);

/// @brief High-level C handler for Overflow Exception (#OF).
/// @param intrrupt Pointer to the saved processor state.
void isr4_handler(interrupt_frames *intrrupt);

/// @brief High-level C handler for BOUND Range Exceeded Exception (#BR).
/// @param intrrupt Pointer to the saved processor state.
void isr5_handler(interrupt_frames *intrrupt);

/// @brief High-level C handler for Invalid Opcode Exception (#UD).
/// @param intrrupt Pointer to the saved processor state.
void isr6_handler(interrupt_frames *intrrupt);

/// @brief High-level C handler for Device Not Available Exception (#NM).
/// @param intrrupt Pointer to the saved processor state.
void isr7_handler(interrupt_frames *intrrupt);

/// @brief High-level C handler for Double Fault Exception (#DF).
/// @param intrrupt Pointer to the saved processor state (includes error code).
void isr8_handler(interrupt_frames *intrrupt);

/// @brief High-level C handler for Invalid TSS Exception (#TS).
/// @param intrrupt Pointer to the saved processor state (includes error code).
void isr10_handler(interrupt_frames *intrrupt);

/// @brief High-level C handler for Segment Not Present (#NP).
/// @param intrrupt Pointer to the saved processor state (includes error code).
void isr11_handler(interrupt_frames *intrrupt);

/// @brief High-level C handler for Stack-Segment Fault (#SS).
/// @param intrrupt Pointer to the saved processor state (includes error code).
void isr12_handler(interrupt_frames *intrrupt);

/// @brief High-level C handler for General Protection Fault (#GP).
/// @param intrrupt Pointer to the saved processor state (includes error code).
void isr13_handler(interrupt_frames *intrrupt);

/// @brief High-level C handler for Page Fault Exception (#PF).
/// @param intrrupt Pointer to the saved processor state (includes error code).
void isr14_handler(interrupt_frames *intrrupt);

/// @brief High-level C handler for x87 FPU Floating-Point Error (#MF).
/// @param intrrupt Pointer to the saved processor state.
void isr16_handler(interrupt_frames *intrrupt);

/// @brief High-level C handler for Alignment Check Exception (#AC).
/// @param intrrupt Pointer to the saved processor state (includes error code).
void isr17_handler(interrupt_frames *intrrupt);

/// @brief High-level C handler for Machine Check Exception (#MC).
/// @param intrrupt Pointer to the saved processor state.
void isr18_handler(interrupt_frames *intrrupt);

/// @brief High-level C handler for SIMD Floating-Point Exception (#XF).
/// @param intrrupt Pointer to the saved processor state.
void isr19_handler(interrupt_frames *intrrupt);

/// @brief High-level C handler for Virtualization Exception (#VE).
/// @param intrrupt Pointer to the saved processor state.
void isr20_handler(interrupt_frames *intrrupt);

/// @brief High-level C handler for Control Protection Exception (#CP).
/// @param intrrupt Pointer to the saved processor state (includes error code).
void isr21_handler(interrupt_frames *intrrupt);

/// @brief High-level C handler for Hypervisor Injection Exception (#HV).
/// @param intrrupt Pointer to the saved processor state.
void isr28_handler(interrupt_frames *intrrupt);

/// @brief High-level C handler for VMM Communication Exception (#VC).
/// @param intrrupt Pointer to the saved processor state (includes error code).
void isr29_handler(interrupt_frames *intrrupt);

/// @brief High-level C handler for Security Exception (#SX).
/// @param intrrupt Pointer to the saved processor state (includes error code).
void isr30_handler(interrupt_frames *intrrupt);

#endif
