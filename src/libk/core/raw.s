.section .text

# void *libk_memcpy(void *dest, const void *src, size_t n);
.global libk_memcpy
.type libk_memcpy, @function
libk_memcpy:
    # rdi: void *dest
    # rsi: const void *src
    # rdx: size_t n

    movq        %rdi, %rax                  # saving the dest in rax to return
    movq        %rdx, %rcx                  # for 'rep movsb', %rcx should hold the size

    cld                                     # setting destination flag to 0. If destination flag is 1,
                                            # the data for dest would go left and vice-versa. So, ...
                                            # setting to 0 since it should go right size.

    rep movsb                               # usin' it!

    ret

# void *libk_memset(void *ptr, char c, size_t n);
.global libk_memset
.type libk_memset, @function
libk_memset:
    # rdi: void *ptr
    # rsi: char c
    # rdx: size_t n

    movq        %rdi, %r8
    movq        %rdx, %rcx
    movb        %sil, %al

    cld
    rep stosb

    mov         %r8, %rax
    ret

# void *libk_memset32(void *ptr, uint32_t c, size_t n);
.global libk_memset32
.type libk_memset32, @function
libk_memset32:
    # rdi: void *ptr
    # rsi: uint32_t c
    # rdx: size_t n

    movq        %rdi, %r8
    movq        %rdx, %rcx
    movl        %esi, %eax

    cld
    rep stosl

    mov         %r8, %rax
    ret
