#include <arch/idt.h>
#include <arch/io.h>
#include <drivers/keyboard/keyboard.h>
#include <kernel/kernel.h>

#define KEYBOARD_DATA_PORT 0x60
#define PIC1_COMMAND_PORT 0x20
#define PIC_EOI 0x20

// shift
#define LSHIFT_PRESS 0x2A
#define RSHIFT_PRESS 0x36
#define LSHIFT_RELEASE 0xAA
#define RSHIFT_RELEASE 0xB6

// caps
#define CAPS_RELEASE 0xBA

// arrows
#define RIGHT_RELEASE 0xCD
#define LEFT_RELEASE 0xCB

static const keyboard_scancodes ps2_set1_normal[128] = {
    [0x00] = KP_UNKNOWN,   [0x01] = KP_ESCAPE, [0x02] = KP_1,
    [0x03] = KP_2,         [0x04] = KP_3,      [0x05] = KP_4,
    [0x06] = KP_5,         [0x07] = KP_6,      [0x08] = KP_7,
    [0x09] = KP_8,         [0x0A] = KP_9,      [0x0B] = KP_0,
    [0x0C] = KP_SUB,       [0x0D] = KP_ADD,    [0x0E] = KP_BSPACE,
    [0x0F] = KP_TAB,       [0x10] = KP_Q,      [0x11] = KP_W,
    [0x12] = KP_E,         [0x13] = KP_R,      [0x14] = KP_T,
    [0x15] = KP_Y,         [0x16] = KP_U,      [0x17] = KP_I,
    [0x18] = KP_O,         [0x19] = KP_P,      [0x1A] = KP_LSQBRA,
    [0x1B] = KP_RSQBRA,    [0x1C] = KP_RETURN, [0x1D] = KP_LCTRL,
    [0x1E] = KP_A,         [0x1F] = KP_S,      [0x20] = KP_D,
    [0x21] = KP_F,         [0x22] = KP_G,      [0x23] = KP_H,
    [0x24] = KP_J,         [0x25] = KP_K,      [0x26] = KP_L,
    [0x27] = KP_SEMICOLON, [0x28] = KP_QUOTE,  [0x29] = KP_BTICK,
    [0x2A] = KP_LSHIFT,    [0x2B] = KP_BSLASH, [0x2C] = KP_Z,
    [0x2D] = KP_X,         [0x2E] = KP_C,      [0x2F] = KP_V,
    [0x30] = KP_B,         [0x31] = KP_N,      [0x32] = KP_M,
    [0x33] = KP_COMMA,     [0x34] = KP_DOT,    [0x35] = KP_FRONTSLASH,
    [0x36] = KP_RSHIFT,    [0x37] = KP_KP_MUL, [0x38] = KP_LALT,
    [0x39] = KP_SPACE,     [0x3A] = KP_CAPS,   [0x3B] = KP_F1,
    [0x3C] = KP_F2,        [0x3D] = KP_F3,     [0x3E] = KP_F4,
    [0x3F] = KP_F5,        [0x40] = KP_F6,     [0x41] = KP_F7,
    [0x42] = KP_F8,        [0x43] = KP_F9,     [0x44] = KP_F10,
    [0x45] = KP_NUMLOCK,   [0x46] = KP_SCROLL, [0x47] = KP_KP_7,
    [0x48] = KP_KP_8,      [0x49] = KP_KP_9,   [0x4A] = KP_KP_MINUS,
    [0x4B] = KP_KP_4,      [0x4C] = KP_KP_5,   [0x4D] = KP_KP_6,
    [0x4E] = KP_KP_ADD,    [0x4F] = KP_KP_1,   [0x50] = KP_KP_2,
    [0x51] = KP_KP_3,      [0x52] = KP_KP_0,   [0x53] = KP_KP_DOT,
    [0x57] = KP_F11,       [0x58] = KP_F12};

static const keyboard_scancodes ps2_set1_extended[128] = {
    [0X1C] = KP_KP_ENTER, [0X1D] = KP_RCTRL,    [0X35] = KP_KP_DIV,
    [0X37] = KP_PRT,      [0X38] = KP_RALT,     [0X47] = KP_HOME,
    [0X48] = KP_UP,       [0X49] = KP_PG_UP,    [0X4B] = KP_LEFT,
    [0X4D] = KP_RIGHT,    [0X4F] = KP_END,      [0X50] = KP_DOWN,
    [0X51] = KP_PG_DN,    [0X52] = KP_INSERT,   [0X53] = KP_DELETE,
    [0X5B] = KP_SUPER,    [0X5D] = KP_CLIPBOARD};

static bool is_extented = false;

// PRIVATE

void keyboard_handler(interrupt_frames *intrupt) {
    unsigned char raw = inb(KEYBOARD_DATA_PORT);
    keyboard_event_t *event = kget_active_keyboard_listener();

    if (raw == 0xE0) {
        is_extented = true;
        outb(PIC1_COMMAND_PORT, PIC_EOI);
        return;
    }

    bool is_release = (raw & 0x80) != 0;
    unsigned char clean_raw = raw & 0x7F;

    keyboard_scancodes scancode = KP_UNKNOWN;
    if (is_extented) {
        scancode = ps2_set1_extended[clean_raw];
        is_extented = false;
    } else {
        scancode = ps2_set1_normal[clean_raw];
    }

    if (scancode != KP_UNKNOWN && event != NULL) {
        event->scancode = scancode;
        event->is_release = is_release;
        event->is_in = !is_release;

        if (scancode == KP_LSHIFT || scancode == KP_RSHIFT) {
            event->shift = !is_release; // True if pressed, false if released
        }

        if (scancode == KP_CAPS && !is_release) {
            event->caps = !event->caps;
        }
    }

    outb(PIC1_COMMAND_PORT, PIC_EOI);
}

// PUBLIC

void keyboard_event_init(keyboard_event_t *event) {
    event->scancode = KP_UNKNOWN;
    event->caps = event->shift = event->num_lock = event->is_release =
        event->is_in = false;
}

void keyboard_init() {
    idt_set_entry(33, isr33, keyboard_handler, 0x8E, 0);

    unsigned char mask = inb(0x21);
    outb(0x21, mask & ~(1 << 1));
}
