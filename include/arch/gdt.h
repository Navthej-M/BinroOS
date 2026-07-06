/// @file
/// @brief Functions to manage GDT

#ifndef _GDT_H
#define _GDT_H

// Selector constants
#define GDT_KCODE 0x08
#define GDT_KDATA 0x10
#define GDT_UCODE 0x1B
#define GDT_UDATA 0x23
#define GDT_TSS 0x28

/// @brief Will load the GDT
extern void gdt_load(void);

#endif
