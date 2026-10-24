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
/// @brief Provide video management

#ifndef _VIDEO_H
#define _VIDEO_H

#include "libk/types.h"

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
