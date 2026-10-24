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
/// @brief Provides various functios by the kernel.

#ifndef _KERNEL_H
#define _KERNEL_H

#include "arch/idt.h"
#include "drivers/keyboard/keyboard.h"
#include "drivers/video/video.h"

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

#define KERNEL_PANIC_MODE_DEBUG 0
#define KERNEL_PANIC_MODE_NORMAL 1

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

/// @brief Will enter the debug console displays details cuz of KERNEL PANIC
///
/// @param mode The kernel panic mode. 0=Debug (minimal, shows registers values)
///             1=NoDebug (just shows a screen of death)
/// @param title The title of the kernel panic
///             [ IMPORTANT IN DEBUG MODE AND OPTIONAL IN NORMAL MODE ]
/// @param frame The interrupt frame (to get values
///             of registers)
///             [ IMPORTANT IN DEBUG MODE AND NOT WON'T CARE IN NORMAL MODE ]
void kernel_panic(uint8_t mode, const char *title, interrupt_frames *frame);

/// @brief Will return required video details (video_driver_t) which is setted
/// up from kvideo_required(...) by _start.
///
/// @return A pointer to the video details
video_driver_t *kget_video_details(void);

/// @return Pointer to the Backbuffer (initilized by kmain)
void *kget_backbuffer(void);

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

/// @brief The kernel starting point which gets called after initlizing all.
void kmain(void);

#endif
