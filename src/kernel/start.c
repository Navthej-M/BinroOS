#include <arch/gdt.h>
#include <arch/idt.h>
#include <arch/io.h>
#include <arch/irq.h>
#include <drivers/keyboard/keyboard.h>
#include <drivers/video/cli/colors.h>
#include <drivers/video/fb.h>
#include <kernel/debug/debug_log/debug_log.h>
#include <kernel/kernel.h>
#include <third_party/limine.h>

static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0,
};

static uint8_t backbuffer[FB_SIZE];
// TODO: Remove; so that use heap insted.

// PRIVATE

extern void pic_remap(void);

// bootloader jumps here
void _start(void) {
    if (framebuffer_request.response == NULL ||
        framebuffer_request.response->framebuffer_count < 1) {
        arch_hang(true);
    }

    struct limine_framebuffer *fb =
        framebuffer_request.response->framebuffers[0];
    kvideo_required(backbuffer, fb->address, fb->width, fb->height, fb->pitch);
    fb_init();

    kdebug_logs_init();

    kdebug_logs_console_clear(color_get_cli_theme_bg(),
                              color_get_cli_theme_fg());
    kdebug_printDONE_msg("Initilized video details");

    gdt_load();
    kdebug_printDONE_msg("Loaded GDT");

    pic_remap();
    kdebug_printDONE_msg("Remapped PIC");

    idt_init();
    kdebug_printDONE_msg("Initilized IDT");

    keyboard_init();
    kdebug_printDONE_msg("Initilized keyboard");

    kinit();
}
