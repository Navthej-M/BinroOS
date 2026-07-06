/// @file
/// @brief Provides various functios by the kernel.

#ifndef _KERNEL_H
#define _KERNEL_H

#include <drivers/keyboard/keyboard.h>
#include <drivers/video/video.h>

// version
#define _STRINGFY_HELPER(x) #x
#define _STRINGFY(x) _STRINGFY_HELPER(x)

#define Kernel_MajorVersion 0
#define Kernel_MinorVersion 1
#define Kernel_PatchVersion 0

#define KernelVersion                                                          \
    _STRINGFY(Kernel_MajorVersion)                                             \
    "." _STRINGFY(Kernel_MinorVersion) "." _STRINGFY(Kernel_PatchVersion)

#define KernelVersionString "Binro Kernel Version " KernelVersion
#define KernelVersionStringSize sizeof(KernelVersionString)

// 1920x1080 * 4 = 8,294,400 bytes (~8.2 MB).
#define FB_SIZE (1920 * 1080 * 4)
// TODO: Remove this so that allocate from heap insted of this hardcoded size.

/// @brief This function should be ran from _start because these details are
/// very important.
///
/// @param backbuffer The backbuffer.
/// @param framebuffer The framebuffer address.
/// @param screen_width The screen width.
/// @param screen_height The screen height.
/// @param pitch The pitch of the display (actual number of bytes a graphics
///              card uses to store one single horizondal row of pixels on the
///              screen)
void kvideo_required(void *backbuffer, void *framebuffer, uint32_t screen_width,
                     uint32_t screen_height, uint32_t pitch);

/// @brief Will return required video details (video_driver_t) which is setted
/// up from kvideo_required(...) by _start.
///
/// @return A pointer to the video details
video_driver_t *kget_video_details(void);

/// @brief Will set the active keyboard listner (keyboard_event_t).
///
/// @param event: The event which is listening.
void kset_active_keyboard_listener(keyboard_event_t *event);

/// @brief Will return the current active keyboard listner's ptr
/// (keyboard_event_t *)
///
/// @return A pointer to the current active keyboard listner (the keyboard
/// event)
keyboard_event_t *kget_active_keyboard_listener(void);

/// @brief  Event gain attention as the main event if it's not until some other
/// does.
///
/// This can be used in a mainloop if you want to know which key is pressed.
/// Event will get updated by the keyboard driver.
///
/// @param event The event to check (and to be updated to the pressed key)
///
/// @return If some key is pressed and event got updated
bool kpoll_event(keyboard_event_t *event);

/// @brief Will initilize the core drivers and stuffs which _start hadn't.
void kinit(void);

/// @brief The kernel starting point which gets called after initlizing all.
void kmain(void);

#endif
