/// @file
/// @brief Contains global requests during boot by the bootloader.

#ifndef _REQUESTS_H
#define _REQUESTS_H

#include <third_party/limine.h>

/// @brief framebuffer request
///
/// @return framebuffer request
struct limine_framebuffer_request request_framebuffer(void);

/// @brief memory map request
///
/// @return memory map request
struct limine_memmap_request request_memap(void);

/// @brief hhdm request
///
/// @return HDDM request
struct limine_hhdm_request request_hhdm(void);

#endif
