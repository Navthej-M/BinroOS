/// @file
/// @brief Provides interface for PMM (Physical Memory Manager)

#ifndef _PMM_H
#define _PMM_H

#include <libk/types.h>

/// @brief Will set {bit} from the bitmap to 1
///
/// @param bit The bit to be 1
void pmm_bitmap_set(size_t bit);

/// @brief Will set {bit} from the bitmap to 0
///
/// @param bit The bit to be 0
void pmm_bitmap_clear(size_t bit);

/// @brief Will return if bit is 0 or 1
///
/// @param bit The bit to check
///
/// @return 1 if {bit} from the bitmap is 1 and 0 if {bit} from the bitmap is 0
bool pmm_bitmap_test(size_t bit);

/// @brief Will initilize PMM
void pmm_init(void);

/// @brief Will return a free page from bitmap and set that free
///
/// @return Pointer to that address
void *pmm_pgalloc(void);

/// @brief Will mark the page as free
void pmm_pgfree(void *addr);

/// @brief Returns the number of free pages
///
/// @return The number of free pages
size_t pmm_get_free_pages(void);

#endif
