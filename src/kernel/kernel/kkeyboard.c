#include <drivers/keyboard/keyboard.h>
#include <kernel/debug/debug_log/debug_log.h>
#include <kernel/kernel.h>

static keyboard_event_t *active_keyboard_event = &(keyboard_event_t){0};

// PRIVATE

void kkeyboard_init(void) {
    keyboard_event_init(active_keyboard_event);
    kset_active_keyboard_listener(active_keyboard_event);
    kdebug_printDONE_msg("Setted up keyboard event");
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
