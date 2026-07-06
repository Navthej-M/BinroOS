/// @file
/// @brief Provide video management

#ifndef _VIDEO_H
#define _VIDEO_H

#include <libk/types.h>

/// @brief The video driver required by the framebuffer.
typedef struct {
    void *backbuffer;  ///< Backbuffer.
    void *framebuffer; ///< Pointer to the framebuffer.
    uint64_t width;    ///< Width of the screen in pixels.
    uint64_t height;   ///< Height of the screen in pixels.
    uint64_t pitch;    ///< Pitch of the display.

    /// @brief Draws a single pixel on the screen.
    ///
    /// @param x The x position of the pixel.
    /// @param y The y position of the pixel.
    /// @param color The color of the pixel.
    void (*draw_pixel)(uint32_t x, uint32_t y, uint32_t color);

    /// @brief Clears the entire screen.
    ///
    /// @param color The color used to fill the cleared screen.
    void (*clear_screen)(uint32_t color);

    /// @brief Will swap the backbuffer with the physical framebuffer.
    void (*swap_buffers)(void);

} video_driver_t;

#endif
