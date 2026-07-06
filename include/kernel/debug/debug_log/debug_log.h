/// @file
/// @brief A temporary, very minimal terminal like environment to display
/// initilization logs by the kernel.

#ifndef _DEBUG_LOG_H
#define _DEBUG_LOG_H

#include <drivers/video/video.h>
#include <libk/types.h>

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
/// This is mostly used by the IDT which will prints the heading using this
/// function and prints it's stuffs and do it's stuffs by itself (like hanging
/// the CPU forever) during interrupts.
///
/// You can also use this to print all details. Like, this is print a heading
/// like KERNEL PANIC and all and then prints the "msg". So, either you can keep
/// msg minimal (only the fault name, like "Page Fault") and print or do other
/// stuffs by yourself (like how IDT does) or you can keep all the text in "msg"
/// to display all message at once.
///
/// @param msg The massage and details of interrupt to display
/// @param clear_screen If screen should be cleared before printing the PANIC.
void kdebug_printINTERRUPT_FAILURE_msg(const char *msg, bool clear_screen);

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
