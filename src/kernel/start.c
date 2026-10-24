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

#include "arch/irq.h"
#include "boot/info.h"
#include "kernel/kernel.h"
#include "third_party/limine.h"

// framebuffer request
static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0,
};

// bootloader jumps here
void _start(void) {
    // Framebuffer and video and pseudo terminal for init logs
    if (framebuffer_request.response == nullptr ||
        framebuffer_request.response->framebuffer_count < 1) {
        arch_hang(true);
    }

    struct limine_framebuffer *fb =
        framebuffer_request.response->framebuffers[0];

    video_info video_i;

    video_i.framebuffer = fb->address;
    video_i.width = fb->width;
    video_i.height = fb->height;
    video_i.pitch = fb->pitch;

    _set_boot_info_((boot_info){
        video_i,
    });

    kmain();
}
