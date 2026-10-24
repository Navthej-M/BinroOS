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
/// @brief Colors and theme management during CLI (no GUI).

#ifndef _COLORS_H
#define _COLORS_H

#include <libk/types.h>

/// @brief Different of colors for the framebuffer.
typedef enum : uint32_t {
    COLOR_BLACK = 0x000000,
    COLOR_BLUE = 0x0000AA,
    COLOR_GREEN = 0x00AA00,
    COLOR_CYAN = 0x00AAAA,
    COLOR_RED = 0xAA0000,
    COLOR_MAGENTA = 0xAA00AA,
    COLOR_BROWN = 0xAA5500,
    COLOR_LIGHT_GRAY = 0xAAAAAA,
    COLOR_DARK_GRAY = 0x555555,
    COLOR_LIGHT_BLUE = 0x5555FF,
    COLOR_LIGHT_GREEN = 0x55FF55,
    COLOR_LIGHT_CYAN = 0x55FFFF,
    COLOR_LIGHT_RED = 0xFF5555,
    COLOR_LIGHT_MAGENTA = 0xFF55FF,
    COLOR_YELLOW = 0xFFFF55,
    COLOR_WHITE = 0xFFFFFF,
    COLOR_CATPPUCCIN_MOCHA_BASE = 0x11111B,
    COLOR_CATPPUCCIN_MOCHA_LAVENDER = 0xB4BEFE,
    COLOR_ORANGE = 0xFF8500,
    COLOR_PURPLE = 0x800080,
    COLOR_PINK = 0xFFC0CB,
    COLOR_TEAL = 0x008080,
    COLOR_OLIVE = 0x808000,
    COLOR_NAVY = 0x000080,
    COLOR_MAROON = 0x800000,
    COLOR_GOLD = 0xFFD700,
    COLOR_SILVER = 0xC0C0C0,
    COLOR_LIME = 0x00FF00,
    COLOR_INDIGO = 0x4B0082,
    COLOR_VIOLET = 0xEE82EE,
    COLOR_SUCCESS = 0x2ECC71,
    COLOR_WARNING = 0xF1C40F,
    COLOR_ERROR = 0xE74C3C,
    COLOR_INFO = 0x3498DB,
    COLOR_CHARCOAL = 0x121212,
    COLOR_SLATE = 0x2C3E50,
    COLOR_DIM_GRAY = 0x696969
} term_color_t;

#endif
