/// @file
/// @brief Provides framebuffer functions.

#ifndef _FB_H
#define _FB_H

#include <libk/types.h>

/// @brief Will initilize the framebuffer.
void fb_init(void);

/// @brief Will chainge color of pixel {x, y} to {color}
///
/// @param x x-coordinate of the pixel
/// @param y y-coordinate of the pixel
/// @param color color for the pixel at {x, y}
void fb_draw_pixel(uint32_t x, uint32_t y, uint32_t color);

/// @brief Will clear the screen and fills the screen with {color}
///
/// @param color The color to fill the screen.
void fb_clear_screen(uint32_t color);

/// @brief Will push the backbuffer to the physical framebuffer
void fb_swap_buffers(void);

#endif
