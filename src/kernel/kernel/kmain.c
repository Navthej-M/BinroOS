#include <arch/irq.h>
#include <drivers/keyboard/keyboard.h>
#include <drivers/video/cli/colors.h>
#include <kernel/debug/debug_log/debug_log.h>
#include <kernel/kernel.h>

extern void kkeyboard_init(void);

void kinit(void) {
    arch_enable_interupts();
    kdebug_printDONE_msg("Intrupts enabled");

    // keyboard event and the active keyboard listener
    kkeyboard_init();

    // jumping to kmain
    kmain();
}

void kmain(void) {
    kdebug_printDONE_msg("Jumped to kmain");
    arch_hang(false);
}
