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
