/// @file
/// @brief Low-level port I/O helper functions.

#ifndef _IO_H
#define _IO_H

#include <libk/types.h>

/// @brief Reads a byte from the specified I/O port.
///
/// @param port The 16-bit I/O port address to read from.
///
/// @return The 8-bit data byte read from the port.
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile("inb %1, %0" : "=a"(ret) : "d"(port));
    return ret;
}

/// @brief Writes a byte to the specified I/O port.
///
/// @param port The 16-bit I/O port address to write to.
/// @param val The 8-bit data byte to write to the port.
static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile("outb %0, %1" : : "a"(val), "d"(port));
}

#endif
