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
/// @brief Contains global requests during boot by the bootloader. By default,
/// LIMINE is chosen as the bootloader. Maybe, currently it aint that effecient
/// to chainge the bootloader since LIMINE is used inside _start. I'll try to
/// change this later cuz, now I only wanna see ts shi boot.

#ifndef _REQUESTS_H
#define _REQUESTS_H

#include "libk/types.h"

/// @brief VIDEO informatin bootloader to be filled
typedef struct {
    // raw details
    void *framebuffer; ///< Raw pointer to the framebuffer

    // display details
    uint64_t width;  ///< Width of the display
    uint64_t height; ///< Height of the display
    uint64_t pitch;  ///< Pitch of the display
} video_info;

/// @brief The entire boot info the kernel expects
typedef struct {
    video_info video; ///< Whole information about video
} boot_info;

/// @brief Use this function to set boot info
///
/// @note This function will track it's callings. If it is getting called more
///       than once, actually, NOTHING happens --- just do nothing. This would
///       be good since unexpected bugs wont be there.
void _set_boot_info_(boot_info info);

/// @return The video details set by the bootloader (setted by _set_boot_info_).
///         Returns an empty struct if boot info was not received.
boot_info get_boot_info(void);

#endif
