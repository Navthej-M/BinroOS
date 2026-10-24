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

#include "kernel/kernel.h"
#include "arch/gdt.h"
#include "arch/irq.h"
#include "arch/tss.h"
#include "boot/info.h"
#include "drivers/keyboard/keyboard.h"
#include "drivers/video/fb.h"
#include "kernel/debug/debug_log/debug_log.h"
#include "libk/core.h"
#include <stdint.h>

// Backbuffer for VIDEO
static void *backbuffer[2480 * 1980];

// for KEYBOARD SERVICES
static keyboard_event_t *active_keyboard_event =
    &(keyboard_event_t){0}; // keyboard current listener
static video_driver_t *kvideo_driver =
    &(video_driver_t){0}; // global video details

// PRIVATE

// align memory to n bytes
static inline size_t align_up(size_t n, size_t size) {
    return (size + (n - 1)) & ~(n - 1);
}

// PUBLIC

void kset_active_keyboard_listener(keyboard_event_t *event) {
    active_keyboard_event = event;
}

keyboard_event_t *kget_active_keyboard_listener(void) {
    return active_keyboard_event;
}

bool kpoll_event(keyboard_event_t *event) {
    if (kget_active_keyboard_listener() != event) {
        kset_active_keyboard_listener(event);
    }

    if (event->is_in) {
        event->is_in = false;
        return true;
    }

    return false;
}

void kvideo_required(void *backbuffer, void *framebuffer, uint32_t screen_width,
                     uint32_t screen_height, uint32_t pitch) {
    kvideo_driver->backbuffer = backbuffer;
    kvideo_driver->framebuffer = framebuffer;
    kvideo_driver->width = screen_width;
    kvideo_driver->height = screen_height;
    kvideo_driver->pitch = pitch;

    kvideo_driver->clear_screen = fb_clear_screen;
    kvideo_driver->draw_pixel = fb_draw_pixel;
    kvideo_driver->swap_buffers = fb_swap_buffers;
}

video_driver_t *kget_video_details(void) { return kvideo_driver; }

void *kget_backbuffer(void) { return backbuffer; }

void kinit(void) {
    // goiing to kernel main function
}

void kernel_panic(uint8_t mode, const char *title, interrupt_frames *frame) {
    typedef struct {
        const char *name;
        uint64_t value;
    } reg_entry_t;

    kdebug_logs_console_clear(kdebug_theme_get_cli_theme_bg(),
                              kdebug_theme_get_cli_theme_fg());

    if (mode == KERNEL_PANIC_MODE_DEBUG) {
        if (!title || !frame) {
            const char error_title[] =
                "\033[31mError from Kernel Panic thrower: \033[0m\n";
            kdebug_puts(error_title, sizeof(error_title));

            if (!title) {
                const char err[] =
                    "\nIn mode 0 (DEBUG) unexpected title NULL.\n";
                kdebug_puts(err, sizeof(err));
            }

            if (!frame) {
                const char err[] =
                    "\nIn mode 0 (DEBUG) unexpected interrupt frames NULL.\n";
                kdebug_puts(err, sizeof(err));
            }

            kdebug_puts("\n", 2);

            const char msg[] = "CPU Hanging...\nPlease restart your computer.";
            kdebug_puts(msg, sizeof(msg));

            arch_hang(true);
        }

        reg_entry_t registers[20] = {
            {"rax", frame->rax}, {"rbx", frame->rbx}, {"rcx", frame->rcx},
            {"rdx", frame->rdx}, {"rsi", frame->rsi}, {"rbp", frame->rbp},
            {"rdi", frame->rdi}, {"r8", frame->r8},   {"r9", frame->r9},
            {"r10", frame->r10}, {"r11", frame->r11}, {"r12", frame->r12},
            {"r13", frame->r13}, {"r14", frame->r14}, {"r15", frame->r15},
            {"rip", frame->rip}, {"cs", frame->cs},   {"rflags", frame->rflags},
            {"rsp", frame->rsp}, {"ss", frame->ss}};

        const uint8_t MAX_REG_IN_ONE_LINE = 2;

        kdebug_printKernelPanic_msg(title, true);
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
                              + 2 // seperator between regs
                              + 1 // null terminator
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
            "\n\n\033[93mTurned off Interrupts. CPU "
            "Hanging.\n\n\033[0mPlease restart your computer.\033";
        kdebug_puts(conclusion, sizeof(conclusion));

        arch_hang(true);
    }

    else {
        // TODO: Make ts into normal string so that compiler will use memcpy().
        // (AFTER MAKING MEMCPY)
        static const char *const logo_ascii =
            "\n\n"
            "\033[91m      __                                               \n"
            "\033[91m       _ \\ \\                                         \n"
            "\033[91m      | | | |                                          \n"
            "\033[91m      | | | |                                          \n"
            "\033[91m      | |/_/    \033[31mKERNEL PANIC  :(               \n"
            "\033[91m      |_|\\ \\  \033[0m  Please restart your computer  \n"
            "\033[91m      | | | |                                          \n"
            "\033[91m      | | | |                                          \n"
            "\033[91m      | |/_/                                           \n"
            "\033[91m      |_|                                              \n"
            "\n\n\n"
            "\033[96m  Something unusual happened which even crashed the "
            "entire "
            "Kernel.\n  Please restart your computer.\n\n\033[31m  ";

        kdebug_puts(logo_ascii, libk_strlen(logo_ascii));

        if (title) {
            kdebug_puts(title, libk_strlen(title));
        }

        arch_hang(true);
    }
}

void kmain(void) {
    // initilizing GDT and IDT
    gdt_load();
    idt_init();

    // Remapping PIC for keyboard
    extern void pic_remap();
    pic_remap();

    // Setting up TSS
    uint64_t kernel_tss_stack[4096]; // TODO: Make it heap allocated
    uint64_t kernel_tss_stack_top = (uint64_t)kernel_tss_stack + 4096;
    tss_init(kernel_tss_stack_top);

    // Initilizing video
    // TODO: backbuffer = allocate for width * height (now it's static, which is
    // not GOOD)
    kvideo_required(kget_backbuffer(), get_boot_info().video.framebuffer,
                    get_boot_info().video.width, get_boot_info().video.height,
                    get_boot_info().video.pitch);
    fb_init();

    // Initilizing console for logs
    kdebug_logs_init();
    kdebug_logs_console_clear(kdebug_theme_get_cli_theme_bg(),
                              kdebug_theme_get_cli_theme_fg());
    kdebug_printDONE_msg("Setted up console for logs");

    // Enabling Intrupts for some drivers to work properly (and mainly, wtf is
    // an OS with no intrrupts)
    arch_enable_interupts();
    kdebug_printDONE_msg("Intrupts enabled");

    // Initilizing the keyboard driver and setting up the first active listener
    // to keytboard intrrupts
    keyboard_init();
    kdebug_printDONE_msg("Initilized keyboard driver");

    keyboard_event_init(active_keyboard_event);
    kset_active_keyboard_listener(active_keyboard_event);
    kdebug_printDONE_msg("Setted up keyboard event");

    // Forever loop (tho never gonna reach here, for safty to not to CRASH by
    // exiting this funtion)
    arch_hang(false);
}
