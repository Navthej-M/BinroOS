#include <drivers/video/fb.h>
#include <kernel/kernel.h>

static video_driver_t *kvideo_driver = &(video_driver_t){0};

void kvideo_required(void *backbuffer, void *framebuffer, uint32_t screen_width,
                     uint32_t screen_height, uint32_t pitch) {
    kvideo_driver->backbuffer = backbuffer;
    kvideo_driver->framebuffer = framebuffer;
    kvideo_driver->width = screen_width;
    kvideo_driver->height = screen_height;
    kvideo_driver->pitch = pitch;

    kvideo_driver->clear_screen = fb_clear_screen;
    kvideo_driver->draw_pixel = fb_draw_pixel;
    kvideo_driver->swap_buffers = fb_swap_buffers;
}

video_driver_t *kget_video_details(void) { return kvideo_driver; }
