#include <arch/io.h>

#define PIC1_COMMAND 0x20
#define PIC1_DATA 0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA 0xA1

void pic_remap(void) {
    // Start initialization (ICW1)
    outb(PIC1_COMMAND, 0x11);
    outb(PIC2_COMMAND, 0x11);

    // Set interrupt offsets (ICW2)
    outb(PIC1_DATA, 0x20); // Master: IRQ0=32, IRQ1=33, ... IRQ7=39
    outb(PIC2_DATA, 0x28); // Slave:  IRQ8=40, IRQ9=41, ... IRQ15=47

    // Set up cascade (ICW3)
    outb(PIC1_DATA, 0x04); // Master has slave on IRQ2
    outb(PIC2_DATA, 0x02); // Slave identity

    // Set 8086 mode (ICW4)
    outb(PIC1_DATA, 0x01);
    outb(PIC2_DATA, 0x01);

    outb(PIC1_DATA, 0xFF);
    outb(PIC2_DATA, 0xFF);
}
