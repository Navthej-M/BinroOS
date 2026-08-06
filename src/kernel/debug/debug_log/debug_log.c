#include <drivers/video/cli/colors.h>
#include <drivers/video/cli/font.h>
#include <kernel/debug/debug_log/debug_log.h>
#include <kernel/kernel.h>
#include <libk/core.h>

static debug_log_t details;

// PRIVATE

static term_color_t CLI_THEME_COLOR_BG = DEBUG_CONSOLE_DEFAULT_BG;
static term_color_t CLI_THEME_COLOR_FG = DEBUG_CONSOLE_DEFAULT_FG;

static void apply_ansi_graphics(void) {
    static const term_color_t standard_fg[] = {
        COLOR_BLACK, COLOR_RED,     COLOR_GREEN, COLOR_BROWN,
        COLOR_BLUE,  COLOR_MAGENTA, COLOR_CYAN,  COLOR_LIGHT_GRAY};

    static const term_color_t bright_fg[] = {
        COLOR_DARK_GRAY,  COLOR_LIGHT_RED,     COLOR_LIGHT_GREEN, COLOR_YELLOW,
        COLOR_LIGHT_BLUE, COLOR_LIGHT_MAGENTA, COLOR_LIGHT_CYAN,  COLOR_WHITE};

    for (int i = 0; i <= details.ansi_argc; i++) {
        uint32_t code = details.ansi_args[i];

        if (code == 0) {
            details.current_fg = kdebug_theme_get_cli_theme_fg();
            details.current_bg = kdebug_theme_get_cli_theme_bg();
        }
        // Standard Colors (30-37 / 40-47)
        else if (code >= 30 && code <= 37) {
            details.current_fg = standard_fg[code - 30];
        } else if (code >= 40 && code <= 47) {
            details.current_bg = standard_fg[code - 40];
        }
        // High-Intensity Bright Colors (90-97 / 100-107)
        else if (code >= 90 && code <= 97) {
            details.current_fg = bright_fg[code - 90];
        } else if (code >= 100 && code <= 107) {
            details.current_bg = bright_fg[code - 100];
        }
        // Extended 8-bit and 24-bit Colors (38 / 48)
        else if ((code == 38 || code == 48) && (i + 1 <= details.ansi_argc)) {
            uint32_t mode = details.ansi_args[i + 1];
            term_color_t extracted_color = COLOR_BLACK;
            bool color_parsed = false;

            // Handle 8-bit Color Palette Lookups (\033[38;5;<index>m)
            if (mode == 5 && (i + 2 <= details.ansi_argc)) {
                uint32_t index = details.ansi_args[i + 2];
                i += 2;
                color_parsed = true;

                if (index < 8) {
                    extracted_color = standard_fg[index];
                } else if (index >= 8 && index < 16) {
                    extracted_color = bright_fg[index - 8];
                } else if (index == 118 || index == 120) {
                    extracted_color = COLOR_LIME;
                } else {
                    extracted_color = COLOR_LIGHT_GRAY;
                }
            }
            // Handle 24-bit TrueColor RGB Lookups (\033[38;2;<r>;<g>;<b>m)
            else if (mode == 2 && (i + 4 <= details.ansi_argc)) {
                uint32_t r = details.ansi_args[i + 2];
                uint32_t g = details.ansi_args[i + 3];
                uint32_t b = details.ansi_args[i + 4];
                i += 4; // r, g, and b arguments
                color_parsed = true;

                // Merging RGB into your hex format (0xRRGGBB)
                extracted_color = (term_color_t)((r << 16) | (g << 8) | b);
            }

            if (color_parsed) {
                if (code == 38) {
                    details.current_fg = extracted_color;
                } else {
                    details.current_bg = extracted_color;
                }
            }
        }
    }
}

// PUBLIC

void kdebug_logs_init(void) {
    details.video = kget_video_details();

    details.cursor_x = details.cursor_y = details.ansi_state =
        details.ansi_argc = 0;

    details.current_fg = kdebug_theme_get_cli_theme_fg();
    details.current_bg = kdebug_theme_get_cli_theme_bg();
}

uint32_t kdebug_theme_get_cli_theme_bg(void) { return CLI_THEME_COLOR_BG; }

uint32_t kdebug_theme_get_cli_theme_fg(void) { return CLI_THEME_COLOR_FG; }

void kdebug_theme_set_cli_theme_bg(term_color_t color) {
    CLI_THEME_COLOR_BG = color;
}

void kdebug_theme_set_cli_theme_fg(term_color_t color) {
    CLI_THEME_COLOR_FG = color;
}

debug_log_t kdebug_logs_get_details(void) { return details; }

void kdebug_logs_console_clear(uint32_t bg, uint32_t fg) {
    details.current_bg = bg;
    details.current_fg = fg;

    details.video->clear_screen(bg);
    details.cursor_x = 0;
    details.cursor_y = 0;

    details.video->swap_buffers();
}

void kdebug_putc(char c) {
    if (details.ansi_state == 1) {
        if (c == '[') {
            details.ansi_state = 2;
            for (int i = 0; i < 8; i++)
                details.ansi_args[i] = 0;
            details.ansi_argc = 0;
            return;
        }
        details.ansi_state = 0;
    } else if (details.ansi_state == 2) {
        if (c >= '0' && c <= '9') {
            details.ansi_args[details.ansi_argc] =
                (details.ansi_args[details.ansi_argc] * 10) + (c - '0');
            return;
        } else if (c == ';') {
            if (details.ansi_argc < 7)
                details.ansi_argc++;
            return;
        } else if (c == 'm') {
            apply_ansi_graphics();
            details.ansi_state = 0;
            return;
        } else if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) {
            details.ansi_state = 0;
            return;
        }
    }

    uint32_t max_x = (details.video->width / DEFAULT_TERMINAL_FONT_WIDTH) *
                     DEFAULT_TERMINAL_FONT_WIDTH;
    uint32_t max_y = (details.video->height / DEFAULT_TERMINAL_FONT_HEIGHT) *
                     DEFAULT_TERMINAL_FONT_HEIGHT;

    switch (c) {
    case '\0':
        return;

    case '\033':
        details.ansi_state = 1;
        return;

    case '\n':
        details.cursor_x = 0;
        details.cursor_y += DEFAULT_TERMINAL_FONT_HEIGHT;

        if (details.cursor_y >= max_y) {
            kdebug_scroll(1);
        }
        return;
    }

    unsigned char uc = (unsigned char)c;
    if (uc > 127) {
        uc = '?';
    }

    const uint16_t *glyph = font_16x32[uc];

    for (uint32_t cy = 0; cy < DEFAULT_TERMINAL_FONT_HEIGHT; cy++) {
        for (uint32_t cx = 0; cx < DEFAULT_TERMINAL_FONT_WIDTH; cx++) {
            if ((glyph[cy] >> ((DEFAULT_TERMINAL_FONT_WIDTH - 1) - cx)) & 1) {
                details.video->draw_pixel(details.cursor_x + cx,
                                          details.cursor_y + cy,
                                          details.current_fg);
            } else {
                details.video->draw_pixel(details.cursor_x + cx,
                                          details.cursor_y + cy,
                                          details.current_bg);
            }
        }
    }

    details.cursor_x += DEFAULT_TERMINAL_FONT_WIDTH;

    if (details.cursor_x >= max_x) {
        details.cursor_x = 0;
        details.cursor_y += DEFAULT_TERMINAL_FONT_HEIGHT;

        if (details.cursor_y >= max_y) {
            kdebug_scroll(1);
        }
    }
}

void kdebug_puts(const char *str, size_t size) {
    for (int i = 0; i < size; ++i) {
        kdebug_putc(str[i]);
    }

    details.video->swap_buffers();
}

void kdebug_printDONE_msg(const char *msg) {
    size_t msg_size = libk_strlen(msg);

    const char label[] = "\033[38;5;118mDONE\033[0m  ";

    char full_msg[sizeof(label) + msg_size + 1];
    size_t ip = 0;

    // label
    for (size_t i = 0; i < sizeof(label); ++i) {
        full_msg[ip++] = label[i];
    }

    // message
    for (size_t i = 0; i < msg_size; ++i) {
        full_msg[ip++] = msg[i];
    }

    // newline
    full_msg[ip++] = '\n';

    kdebug_puts(full_msg, ip);
}

void kdebug_printFAIL_msg(const char *msg) {
    size_t msg_size = libk_strlen(msg);

    const char label[] = "\033[31mFAIL\033[0m  ";

    char full_msg[sizeof(label) + msg_size + 1];
    size_t ip = 0;

    // label
    for (size_t i = 0; i < sizeof(label); ++i) {
        full_msg[ip++] = label[i];
    }

    // message
    for (size_t i = 0; i < msg_size; ++i) {
        full_msg[ip++] = msg[i];
    }

    // newline
    full_msg[ip++] = '\n';

    kdebug_puts(full_msg, ip);
}

void kdebug_printKernelPanic_msg(const char *panic_title, bool clear_screen) {
    size_t msg_size = libk_strlen(panic_title);
    size_t ip = 0;

    if (clear_screen) {
        kdebug_logs_console_clear(kdebug_theme_get_cli_theme_bg(),
                                  kdebug_theme_get_cli_theme_fg());
    } else {
        kdebug_putc('\n');
    }

    const char title[] = "KERNEL PANIC (DEBUG MODE)";

    char red[] = "\033[91m";
    char dred[] = "\033[31m";
    char lgrey[] = "\033[37m";
    char reset[] = "\033[0m";

    size_t screen_pixels = kget_video_details()->width;
    size_t font_pizels = sizeof(title) * DEFAULT_TERMINAL_FONT_WIDTH;
    size_t left_margin_pixels = (screen_pixels - font_pizels) / 2;
    size_t spaces_required = left_margin_pixels / DEFAULT_TERMINAL_FONT_WIDTH;

    size_t buf_size =
        sizeof(lgrey) - 1                             // lgrey color for '-'
        + screen_pixels / DEFAULT_TERMINAL_FONT_WIDTH // for '-' in whole line
        + spaces_required                             // ' ' until title
        + sizeof(dred) - 1                            // dred color for title
        + sizeof(title) - 1                           // title
        + 1                                           // ' ' for the newline
        + sizeof(lgrey)                               // lgrey color for '-'
        + screen_pixels / DEFAULT_TERMINAL_FONT_WIDTH // for '-' in whole line
        + sizeof(dred) - 1                            // for the message
        + msg_size                                    // medssage
        + sizeof(reset) - 1                           // resetting the colors
        + 1                                           // for the finel newline
        + 1                                           // null terminator
        ;

    char full_text[buf_size];

    // lgrey color for '-'
    for (size_t i = 0; i < sizeof(lgrey); ++i) {
        full_text[ip++] = lgrey[i];
    }

    // the '-' in line 1
    for (size_t i = 0; i < screen_pixels / DEFAULT_TERMINAL_FONT_WIDTH; ++i) {
        full_text[ip++] = '-';
    }

    // the ' ' until the center
    for (size_t i = 0; i < spaces_required; ++i) {
        full_text[ip++] = ' ';
    }

    // dred
    for (size_t i = 0; i < sizeof(dred); ++i) {
        full_text[ip++] = dred[i];
    }

    // title
    for (size_t i = 0; i < sizeof(title); ++i) {
        full_text[ip++] = title[i];
    }

    // lgrey color for '-'
    for (size_t i = 0; i < sizeof(lgrey); ++i) {
        full_text[ip++] = lgrey[i];
    }

    // the '\n'
    full_text[ip++] = '\n';

    // the '-' in line 3
    for (size_t i = 0; i < screen_pixels / DEFAULT_TERMINAL_FONT_WIDTH; ++i) {
        full_text[ip++] = '-';
    }

    // setting color for thhe msg
    for (size_t i = 0; i < sizeof(red); ++i) {
        full_text[ip++] = red[i];
    }

    // newline
    full_text[ip++] = '\n';

    // msg
    for (size_t i = 0; i < msg_size; ++i) {
        full_text[ip++] = panic_title[i];
    }

    // resetting color to normals
    for (size_t i = 0; i < sizeof(reset); ++i) {
        full_text[ip++] = reset[i];
    }

    // null terminator
    full_text[ip++] = '\0';

    kdebug_puts(full_text, ip);
}

void kdebug_scroll(size_t lines_to_scroll) {
    size_t bytes_per_line = details.video->pitch * DEFAULT_TERMINAL_FONT_HEIGHT;
    size_t total_lines = details.video->height / DEFAULT_TERMINAL_FONT_HEIGHT;

    if (lines_to_scroll > total_lines) {
        lines_to_scroll = total_lines;
    }

    void *src = (uint8_t *)details.video->backbuffer +
                (lines_to_scroll * bytes_per_line);
    size_t move_bytes = bytes_per_line * (total_lines - lines_to_scroll);

    libk_memmove(details.video->backbuffer, src, move_bytes);

    void *dst_clear = (uint8_t *)details.video->backbuffer + move_bytes;
    size_t clear_bytes = bytes_per_line * lines_to_scroll;
    size_t clear_pixels = clear_bytes / 4;

    libk_memset32(dst_clear, kdebug_theme_get_cli_theme_bg(), clear_pixels);

    if (details.cursor_y >= DEFAULT_TERMINAL_FONT_HEIGHT * lines_to_scroll) {
        details.cursor_y -= DEFAULT_TERMINAL_FONT_HEIGHT * lines_to_scroll;
    } else {
        details.cursor_y = 0;
    }
    details.cursor_x = 0;

    details.video->swap_buffers();
}
