#include <drivers/video/video.h>
#include <kernel/kernel.h>
#include <libk/kstring.h>
#include <libk/types.h>

static void *fb_physical = NULL;
static void *fb_backbuffer = NULL;

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
