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

#include "drivers/video/video.h"
#include "kernel/kernel.h"
#include "libk/core.h"
#include "libk/types.h"

static void *fb_physical = nullptr;
static void *fb_backbuffer = nullptr;

static uint64_t fb_width = 0;
static uint64_t fb_height = 0;
static uint64_t fb_pitch = 0;

void fb_init(void) {
    fb_physical = kget_video_details()->framebuffer;
    fb_backbuffer = kget_video_details()->backbuffer;

    fb_width = kget_video_details()->width;
    fb_height = kget_video_details()->height;
    fb_pitch = kget_video_details()->pitch;
}

void fb_draw_pixel(uint32_t x, uint32_t y, uint32_t color) {
    if (x >= fb_width || y >= fb_height) {
        return;
    }

    uint32_t *pixel =
        (uint32_t *)((uint8_t *)fb_backbuffer + (y * fb_pitch) + (x * 4));
    *pixel = color;
}

void fb_clear_screen(uint32_t color) {
    libk_memset32(fb_backbuffer, color, (fb_pitch * fb_height) / 4);
}

void fb_swap_buffers(void) {
    libk_memcpy(fb_physical, fb_backbuffer, fb_pitch * fb_height);
}
