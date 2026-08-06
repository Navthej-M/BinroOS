#include <arch/gdt.h>
#include <arch/idt.h>
#include <arch/io.h>
#include <arch/irq.h>
#include <drivers/keyboard/keyboard.h>
#include <drivers/video/cli/colors.h>
#include <drivers/video/fb.h>
#include <kernel/debug/debug_log/debug_log.h>
#include <kernel/kernel.h>
#include <kernel/mm/pmm.h>
#include <kernel/mm/vmm.h>
#include <requests.h>

uint8_t *backbuffer = nullptr;

// framebuffer request
static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0,
};

// memory map request
static volatile struct limine_memmap_request memap_request = {
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 0,
};

// hhdm request
static volatile struct limine_hhdm_request hhdm_request = {
    .id = LIMINE_HHDM_REQUEST,
    .revision = 0,
};

struct limine_framebuffer_request request_framebuffer(void) {
    return framebuffer_request;
}

struct limine_memmap_request request_memap(void) { return memap_request; }

struct limine_hhdm_request request_hhdm(void) { return hhdm_request; }

// bootloader jumps here
void _start(void) {
    // Core Memory & CPU initilization
    pmm_init();
    vmm_init();
    kheap_init();

    gdt_load();
    idt_init();

    extern void pic_remap();
    pic_remap();

    // Framebuffer and video and pseudo terminal for init logs
    if (request_framebuffer().response == nullptr ||
        request_framebuffer().response->framebuffer_count < 1) {
        arch_hang(true);
    }

    struct limine_framebuffer *fb =
        request_framebuffer().response->framebuffers[0];

    backbuffer = kmalloc(fb->height * fb->pitch);
    if (!backbuffer) {
        kernel_panic(KERNEL_PANIC_MODE_NORMAL,
                     "kmalloc failed! Heap too small for backbuffer", nullptr);
        arch_hang(true);
    }

    kvideo_required(backbuffer, fb->address, fb->width, fb->height, fb->pitch);
    fb_init();

    kdebug_logs_init();
    kdebug_logs_console_clear(kdebug_theme_get_cli_theme_bg(),
                              kdebug_theme_get_cli_theme_fg());
    kdebug_printDONE_msg("Setted up pseudo console for logs");

    kinit();
    kmain();
}
