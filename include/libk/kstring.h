/// @file
/// @brief Provides helpers to do string related stuffs.

#ifndef _STRING_H
#define _STRING_H

#include <libk/types.h>

/// @brief Will calculate str's length.
///
/// @param str string to get the size of
///
/// @return The length of str.
size_t libk_strlen(const char *str);

/// @brief Will copy src's n bytes to dest.
///
/// It won't modify the original src pointer btw. This will also return a
/// pointer to dest.
///
/// @param dest destination for the bytes to get copied
/// @param src the source bytes
/// @param n the number of bytes to get copied to dest
void *libk_memcpy(void *dest, const void *src, size_t n);

/// @brief Will copy src's n bytes to dest by handeling memory overlapping.
///
/// Unlike libk_memcpy, which would act unexpectedly in some cases cuz of
/// overlapping, this will prevent overlapping and make it work in all cases. It
/// won't modify the original src pointer btw.
///
/// This also returns a pointer to dest.
///
/// @param dest destination for the bytes to get copied
/// @param src the source bytes
/// @param n the n of bytes to get copied to dest
void *libk_memmove(void *dest, const void *src, size_t n);

/// @brief Will set ptr's n bytes into c.
///
/// This will also return a pointer to dest.
///
/// @param ptr pointer to where the bytes to get changed
/// @param c the new bytes to put to the address
/// @param n the number of times the loop to repeat
void *libk_memset(void *ptr, char c, size_t n);

/// @brief 32-bit version of libk_memcpy.
///
/// This will set ptr's n bytes into c and return a pointer to dest.
///
/// @param ptr pointer to where the bytes to get changed
/// @param c the new bytes to put to the address
/// @param n the number of times the loop to repeat
void *libk_memset32(void *ptr, uint32_t c, size_t n);

#endif
