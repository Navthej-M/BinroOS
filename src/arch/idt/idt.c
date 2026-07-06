#include <arch/gdt.h>
#include <arch/idt.h>
#include <arch/irq.h>
#include <kernel/debug/debug_log/debug_log.h>
#include <kernel/kernel.h>
#include <libk/kcast.h>
#include <libk/kstring.h>

static idt_entry idt[IDT_ENTRIES];
static idt_ptr ptr;

// PRIVATE

typedef struct {
    const char *name;
    uint64_t value;
} reg_entry_t;

void interrupt_fault(const char *fault_title, interrupt_frames *frame) {
    reg_entry_t registers[20] = {
        {"rax", frame->rax}, {"rbx", frame->rbx}, {"rcx", frame->rcx},
        {"rdx", frame->rdx}, {"rsi", frame->rsi}, {"rbp", frame->rbp},
        {"rdi", frame->rdi}, {"r8", frame->r8},   {"r9", frame->r9},
        {"r10", frame->r10}, {"r11", frame->r11}, {"r12", frame->r12},
        {"r13", frame->r13}, {"r14", frame->r14}, {"r15", frame->r15},
        {"rip", frame->rip}, {"cs", frame->cs},   {"rflags", frame->rflags},
        {"rsp", frame->rsp}, {"ss", frame->ss}};

    const uint8_t MAX_REG_IN_ONE_LINE = 2;

    kdebug_printINTERRUPT_FAILURE_msg(fault_title, true);
    kdebug_puts("\n\n\033[92mRegisters:\n\033[0m", 22);

    const char reset_color[] = "\033[0m";
    const char reg_color[] = "\033[95m";
    const char num_color[] = "\033[36m";

    for (size_t i = 0, j = 1; i < 20; ++i) {
        const char *reg_name = registers[i].name;
        size_t reg_name_size = libk_strlen(reg_name);

        char reg_value[25];
        size_t value_size = libk_u64_to_str(reg_value, registers[i].value);

        size_t buf_size = sizeof(reg_color) - 1     // color for reg name
                          + reg_name_size           // reg name
                          + sizeof(reset_color) - 1 // color reset
                          + 2                       // border
                          + sizeof(num_color) - 1   // color for reg value
                          + value_size              // value size of the reg
                          + sizeof(reset_color) - 1 // resetting color
                          + 2                       // seperator between regs
                          + 1                       // null terminator
            ;

        char full_details[buf_size];
        size_t ip = 0;

        // color for register name
        for (size_t k = 0; k < sizeof(reg_color) - 1; ++k) {
            full_details[ip++] = reg_color[k];
        }

        // register name
        for (size_t k = 0; k < reg_name_size; ++k) {
            full_details[ip++] = reg_name[k];
        }

        // resetting color
        for (size_t k = 0; k < sizeof(reset_color) - 1; ++k) {
            full_details[ip++] = reset_color[k];
        }

        // border between register name and value (size=2)
        full_details[ip++] = ':';
        full_details[ip++] = ' ';

        // color for register value
        for (size_t k = 0; k < sizeof(num_color) - 1; ++k) {
            full_details[ip++] = num_color[k];
        }

        // register value
        for (size_t k = 0; k < value_size; ++k) {
            full_details[ip++] = reg_value[k];
        }

        // resetting color
        for (size_t k = 0; k < sizeof(reset_color) - 1; ++k) {
            full_details[ip++] = reset_color[k];
        }

        // seperator between registers (size=2)
        full_details[ip++] = ',';
        full_details[ip++] = ' ';

        // null terminator
        full_details[ip++] = '\0';

        kdebug_puts(full_details, ip);

        if (j >= MAX_REG_IN_ONE_LINE) {
            kdebug_putc('\n');
            j = 1;
        } else {
            ++j;
        }
    }

    char ec_str[4];
    size_t ec_str_size = libk_u64_to_str(ec_str, frame->error_code);
    char ec_label[] = "\033[92mError Code:\033[0m ";

    size_t buf_size_ec_details = 1                          // newline
                                 + sizeof(ec_label)         // label of ec
                                 + sizeof(num_color) - 1    // ec color
                                 + ec_str_size              // for the ec
                                 + sizeof(reset_color) - 1; // reset color

    char full_ec_details[buf_size_ec_details];
    size_t ip_for_ec = 0;

    // newline
    full_ec_details[ip_for_ec++] = '\n';

    // label
    for (size_t i = 0; i < sizeof(ec_label); ++i) {
        full_ec_details[ip_for_ec++] = ec_label[i];
    }

    // ec color
    for (size_t i = 0; i < sizeof(num_color); ++i) {
        full_ec_details[ip_for_ec++] = num_color[i];
    }

    // ec
    for (size_t i = 0; i < ec_str_size; ++i) {
        full_ec_details[ip_for_ec++] = ec_str[i];
    }

    // reset color
    for (size_t i = 0; i < sizeof(reset_color); ++i) {
        full_ec_details[ip_for_ec++] = reset_color[i];
    }

    kdebug_puts(full_ec_details, ip_for_ec);

    const char conclusion[] =
        "\n\n\n\033[93mTurned off Interrupts. CPU Hanging.\033[0m";
    kdebug_puts(conclusion, sizeof(conclusion));

    arch_hang(true);
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
