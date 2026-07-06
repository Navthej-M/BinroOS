/// @file
/// @brief Helpers for casting diffrent types.

#ifndef _KCAST_H
#define _KCAST_H

#include <libk/types.h>

/// @brief Will convert a 64-bit number into a string.
///
/// This function will convert a 64-bit number into a string and stores inside
/// the buf with the null terminator in it at the end.
///
/// @param buf The buffer where string should be stored
/// @param num The number to get converted into string
///
/// @return The size of the string.
size_t libk_u64_to_str(char buf[], uint64_t num) {
    size_t idx = 0;

    if (num == 0) {
        buf[idx++] = '0';
        buf[idx] = '\0';
        return idx;
    }

    size_t start_idx = idx;
    while (num > 0) {
        buf[idx++] = (char)('0' + (num % 10));
        num /= 10;
    }

    size_t end_idx = idx - 1;
    while (start_idx < end_idx) {
        char temp = buf[start_idx];
        buf[start_idx] = buf[end_idx];
        buf[end_idx] = temp;
        start_idx++;
        end_idx--;
    }

    buf[idx] = '\0';
    return idx;
}

#endif
