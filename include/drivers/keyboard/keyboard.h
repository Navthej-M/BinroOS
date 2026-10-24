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
/// @brief Provides keyboard and events interface

#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include <libk/types.h>

/// @brief This enum contains all key list
typedef enum {
    KP_UNKNOWN = 0,

    // Main Alphabets
    KP_A,
    KP_B,
    KP_C,
    KP_D,
    KP_E,
    KP_F,
    KP_G,
    KP_H,
    KP_I,
    KP_J,
    KP_K,
    KP_L,
    KP_M,
    KP_N,
    KP_O,
    KP_P,
    KP_Q,
    KP_R,
    KP_S,
    KP_T,
    KP_U,
    KP_V,
    KP_W,
    KP_X,
    KP_Y,
    KP_Z,

    // Main Numbers
    KP_0,
    KP_1,
    KP_2,
    KP_3,
    KP_4,
    KP_5,
    KP_6,
    KP_7,
    KP_8,
    KP_9,

    // Dedicated Navigation & Editing Cluster
    KP_INSERT,
    KP_HOME,
    KP_PG_UP,
    KP_DELETE,
    KP_END,
    KP_PG_DN,
    KP_UP,
    KP_DOWN,
    KP_LEFT,
    KP_RIGHT,

    // Standard Numeric Keypad (Numpad) Keys
    KP_KP_0,
    KP_KP_1,
    KP_KP_2,
    KP_KP_3,
    KP_KP_4,
    KP_KP_5,
    KP_KP_6,
    KP_KP_7,
    KP_KP_8,
    KP_KP_9,
    KP_KP_MINUS,
    KP_KP_ADD,
    KP_KP_MUL,
    KP_KP_DIV,
    KP_KP_DOT,
    KP_KP_ENTER,

    // System & Modifiers
    KP_NUMLOCK,
    KP_CAPS,
    KP_LSHIFT,
    KP_RSHIFT,
    KP_LCTRL,
    KP_RCTRL,
    KP_LALT,
    KP_RALT,
    KP_SUPER,
    KP_TAB,
    KP_BSPACE,
    KP_RETURN,
    KP_ESCAPE,

    // Function Keys
    KP_F1,
    KP_F2,
    KP_F3,
    KP_F4,
    KP_F5,
    KP_F6,
    KP_F7,
    KP_F8,
    KP_F9,
    KP_F10,
    KP_F11,
    KP_F12,

    // Control & Utilities
    KP_PRT,
    KP_SCROLL,
    KP_PAUSE,
    KP_CLIPBOARD,

    // Main Symbols (Physical keys only)
    KP_COMMA,
    KP_DOT,
    KP_SUB,
    KP_ADD,
    KP_BTICK,
    KP_LSQBRA,
    KP_RSQBRA,
    KP_FRONTSLASH,
    KP_SEMICOLON,
    KP_QUOTE,
    KP_BSLASH,
    KP_SPACE,

    KP_MAX_KEYS
} keyboard_scancodes;

/// @brief Event for keyboard.
typedef struct {
    keyboard_scancodes scancode; ///< The actual pressed key
    bool shift;                  ///< If shift is on
    bool caps;                   ///< If cpas is on
    bool num_lock;               ///< If Num Lock is on
    bool is_release;             ///< Is a key released
    bool is_in;                  ///< Is a key pressed
} keyboard_event_t;

/// @brief Will initialize an event.
///
/// @param event The event to get initilized.
void keyboard_event_init(keyboard_event_t *event);

/// @brief Will initialize keyboard hardware.
void keyboard_init(void);

#endif
