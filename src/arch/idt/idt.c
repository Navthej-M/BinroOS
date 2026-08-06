#include <arch/gdt.h>
#include <arch/idt.h>
#include <arch/irq.h>
#include <kernel/debug/debug_log/debug_log.h>
#include <kernel/kernel.h>

static idt_entry idt[IDT_ENTRIES];
static idt_ptr ptr;

// PRIVATE

void interrupt_fault(const char *fault_title, interrupt_frames *frame) {
    kernel_panic(KERNEL_PANIC_MODE_DEBUG, fault_title, frame);
}

// the functions array
void (*handler_functions[IDT_ENTRIES])(interrupt_frames *) = {0};

extern void idt_load(uint64_t idt_ptr_addr);

void isr_common_interrupt_dispatcher(interrupt_frames *interrupt) {
    void (*handler)(interrupt_frames *) = handler_functions[interrupt->vector];

    if (handler) {
        handler(interrupt);
    }
}

// PUBLIC

void idt_init(void) {
    for (int i = 0; i < IDT_ENTRIES; ++i) {
        idt_set_entry(i, (void *)0, (void *)0, 0, 0);
    }

    ptr.limit = sizeof(idt) - 1;
    ptr.base = (uint64_t)&idt;

    idt_load((uint64_t)&ptr);

    idt_set_entry(0, isr0, isr0_handler, 0x8E, 0);
    idt_set_entry(1, isr1, isr1_handler, 0x8E, 0);
    idt_set_entry(2, isr2, isr2_handler, 0x8E, 0);
    idt_set_entry(3, isr3, isr3_handler, 0x8E, 0);
    idt_set_entry(4, isr4, isr4_handler, 0x8E, 0);
    idt_set_entry(5, isr5, isr5_handler, 0x8E, 0);
    idt_set_entry(6, isr6, isr6_handler, 0x8E, 0);
    idt_set_entry(7, isr7, isr7_handler, 0x8E, 0);
    idt_set_entry(8, isr8, isr8_handler, 0x8E, 0);
    idt_set_entry(10, isr10, isr10_handler, 0x8E, 0);
    idt_set_entry(11, isr11, isr11_handler, 0x8E, 0);
    idt_set_entry(12, isr12, isr12_handler, 0x8E, 0);
    idt_set_entry(13, isr13, isr13_handler, 0x8E, 0);
    idt_set_entry(14, isr14, isr14_handler, 0x8E, 0);
    idt_set_entry(16, isr16, isr16_handler, 0x8E, 0);
    idt_set_entry(17, isr17, isr17_handler, 0x8E, 0);
    idt_set_entry(18, isr18, isr18_handler, 0x8E, 0);
    idt_set_entry(19, isr19, isr19_handler, 0x8E, 0);
    idt_set_entry(20, isr20, isr20_handler, 0x8E, 0);
    idt_set_entry(21, isr21, isr21_handler, 0x8E, 0);
    idt_set_entry(28, isr28, isr28_handler, 0x8E, 0);
    idt_set_entry(29, isr29, isr29_handler, 0x8E, 0);
    idt_set_entry(30, isr30, isr30_handler, 0x8E, 0);
}

void idt_set_entry(uint8_t vector, void *isr_handler,
                   void (*handler)(interrupt_frames *), uint8_t type_attr,
                   uint8_t ist) {
    if (ist > 7) {
        return;
    }

    uint64_t addr = (uint64_t)isr_handler;
    idt[vector].base_low = addr & 0xFFFF;
    idt[vector].selector = GDT_KCODE;
    idt[vector].ist = ist;
    idt[vector].type_attr = type_attr;
    idt[vector].base_mid = (addr >> 16) & 0xFFFF;
    idt[vector].base_high = (addr >> 32) & 0xFFFFFFFF;
    idt[vector].reserved = 0;

    if (handler) {
        idt_set_handler_function(vector, handler);
    }
}

void idt_set_handler_function(uint8_t vector,
                              void (*handler)(interrupt_frames *)) {
    handler_functions[vector] = handler;
}

// HANDLERS

void isr0_handler(interrupt_frames *interrupt) {
    interrupt_fault("Divide by Zero", interrupt);
}

void isr1_handler(interrupt_frames *interrupt) { arch_hang(true); }

void isr2_handler(interrupt_frames *interrupt) { arch_hang(true); }

void isr3_handler(interrupt_frames *interrupt) { arch_hang(true); }

void isr4_handler(interrupt_frames *interrupt) {
    interrupt_fault("Overflow", interrupt);
}

void isr5_handler(interrupt_frames *interrupt) {
    interrupt_fault("Bound Range Exeed", interrupt);
}

void isr6_handler(interrupt_frames *interrupt) {
    interrupt_fault("Invalid OPCode", interrupt);
}

void isr7_handler(interrupt_frames *interrupt) {
    interrupt_fault("Device Not Found", interrupt);
}

void isr8_handler(interrupt_frames *interrupt) {
    interrupt_fault("Double Fault", interrupt);
}

void isr10_handler(interrupt_frames *interrupt) {
    interrupt_fault("Invalid TSS", interrupt);
}

void isr11_handler(interrupt_frames *interrupt) {
    interrupt_fault("Segment Not Present", interrupt);
}

void isr12_handler(interrupt_frames *interrupt) {
    interrupt_fault("Stack Segment Fault", interrupt);
}

void isr13_handler(interrupt_frames *interrupt) {
    interrupt_fault("General Protection Fault", interrupt);
}

void isr14_handler(interrupt_frames *interrupt) {
    interrupt_fault("Page Fault", interrupt);
}

void isr16_handler(interrupt_frames *interrupt) {
    interrupt_fault("X87 Floating Point Exception", interrupt);
}

void isr17_handler(interrupt_frames *interrupt) { arch_hang(true); }

void isr18_handler(interrupt_frames *interrupt) { arch_hang(true); }

void isr19_handler(interrupt_frames *interrupt) {
    interrupt_fault("SIMD Floating Point Exception", interrupt);
}

void isr20_handler(interrupt_frames *interrupt) {
    interrupt_fault("Virtualization Exception", interrupt);
}

void isr21_handler(interrupt_frames *interrupt) {
    interrupt_fault("Control Protection Exception", interrupt);
}

void isr28_handler(interrupt_frames *interrupt) {
    interrupt_fault("Hypervisor Injection Exception", interrupt);
}

void isr29_handler(interrupt_frames *interrupt) {
    interrupt_fault("VMM Communication Exception", interrupt);
}

void isr30_handler(interrupt_frames *interrupt) {
    interrupt_fault("Security Exception", interrupt);
}
