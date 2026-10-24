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
/// @brief A temporary, very minimal terminal like environment to display
/// initilization logs by the kernel.

#ifndef _DEBUG_LOG_H
#define _DEBUG_LOG_H

#include "drivers/video/cli/colors.h"
#include "drivers/video/video.h"
#include "libk/types.h"

#define DEBUG_CONSOLE_DEFAULT_BG COLOR_CATPPUCCIN_MOCHA_BASE
#define DEBUG_CONSOLE_DEFAULT_FG COLOR_CATPPUCCIN_MOCHA_LAVENDER

/// @brief Holds the important details needed to print logs.
typedef struct {
    uint32_t cursor_x;     ///< The cursor x position
    uint32_t cursor_y;     ///< The cursor y position
    uint32_t current_fg;   ///< Current text color
    uint32_t current_bg;   ///< Current background color
    video_driver_t *video; ///< Video details

    /// @name For ANSI Parsing, automatically managed
    /// @{
    uint8_t ansi_state;
    uint32_t ansi_args[8];
    uint8_t ansi_argc;
    /// @}
} debug_log_t;

/// @brief Will initilize the details for the debug logs (the temporary minimal
/// terminal-like environment)
void kdebug_logs_init(void);

/// @brief Will return CLI THEME background color.
///
/// @return The CLI THEME bg which is setted up using color_set_cli_theme_bg.
uint32_t kdebug_theme_get_cli_theme_bg(void);

/// @brief Will return CLI THEME fg color.
///
/// @return The CLI THEME fg which is setted up using color_set_cli_theme_fg.
uint32_t kdebug_theme_get_cli_theme_fg(void);

/// @brief Will set the CLI THEME bg to {color}
///
/// @param color The color to be setted as the theme bg.
void kdebug_theme_set_cli_theme_bg(term_color_t color);

/// @brief Will set the CLI THEME fg to {color}
///
/// @param color The color to be setted as the theme fg.
void kdebug_theme_set_cli_theme_fg(term_color_t color);

/// @brief Returns the debug_log_t used by the kernel.
///
/// @return The debug temporary console details (debug_log_t)
debug_log_t kdebug_logs_get_details(void);

/// @brief Will clear the screen.
///
/// @param bg background color for the clean console
/// @param fg text color to use after clearing
void kdebug_logs_console_clear(uint32_t bg, uint32_t fg);

/// @brief Will print a character to the colsole directlty using
/// framebuffer.
///
/// @param c The character to print.
void kdebug_putc(char c);

/// @brief Will print a log by directlty talking to the framebuffer.
///
/// @param str The string to print.
/// @param size Size of the string.
void kdebug_puts(const char *str, size_t size);

/// @brief A wrapper around kdebug_puts and prints the message with some labels
/// to show it's DONE.
///
/// @param msg The DONE message to display
void kdebug_printDONE_msg(const char *msg);

/// @brief A wrapper around kdebug_puts and prints the message with some labels
/// to show it's FAIL.
///
/// @param msg The FAIL message to display
void kdebug_printFAIL_msg(const char *msg);

/// @brief A wrapper around kdebug_puts and kdebug_logs_console_clear (optional
/// btw) to show the interrupt failed message.
///
/// This will just display KERNEL PANIC heading, and {title}. Then it's upto
/// you. Print, hang, do whatever.
///
/// @param title The title for the kernel panic msg
/// @param clear_screen If screen should be cleared before printing the PANIC.
void kdebug_printKernelPanic_msg(const char *title, bool clear_screen);

/// @brief Will scroll the screen lines_to_scroll times
///
/// This directlty chainges the framebuffer via memcpy and all. So, the
/// scrolling would be very fast. However, it had to use classic for-loops to
/// replace the last line, which, may would cause a slight noticable delay
/// without optimizations and all.
///
/// @param lines_to_scroll how much lines to scroll.
void kdebug_scroll(size_t lines_to_scroll);

#endif
