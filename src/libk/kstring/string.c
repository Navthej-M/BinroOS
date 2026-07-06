#include <libk/kstring.h>
#include <stdint.h>

size_t libk_strlen(const char *str) {
    size_t size = 0;
    while (str[size++] != 0x00)
        ;

    return size;
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
