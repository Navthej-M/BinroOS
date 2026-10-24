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

#include "libk/core.h"

size_t libk_strlen(const char *str) {
    size_t size = 0;
    while (str[size++] != 0x00)
        ;

    return size;
}

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

void *libk_memmove(void *dest, const void *src, size_t size) {
    if (dest == src || size == 0) {
        return dest;
    }

    if (dest < src) {
        libk_memcpy(dest, src, size);
    } else {
        char *d = (char *)dest + size - 1;
        const char *s = (const char *)src + size - 1;
        while (size--) {
            *d-- = *s--;
        }
    }
    return dest;
}
