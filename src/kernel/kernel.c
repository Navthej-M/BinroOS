#include <arch/irq.h>
#include <drivers/keyboard/keyboard.h>
#include <drivers/video/fb.h>
#include <kernel/debug/debug_log/debug_log.h>
#include <kernel/kernel.h>
#include <kernel/mm/pmm.h>
#include <kernel/mm/vmm.h>
#include <libk/core.h>

#define HEAP_MAGIC 0xC001BEEF
#define LARGE_ALLOC_MAGIC 0xB16B100C

typedef struct heap_block {
    size_t size;             // size of the usable memory
    bool is_free;            // available?
    uint32_t magic;          // magic number
    struct heap_block *next; // next block to the heap
} heap_block;

heap_block *heap_head = nullptr; // heap head linked list starts here

static keyboard_event_t *active_keyboard_event =
    &(keyboard_event_t){0}; // keyboard current listener
static video_driver_t *kvideo_driver =
    &(video_driver_t){0}; // global video details

// PRIVATE

// align memory to n bytes
static inline size_t align_up(size_t n, size_t size) {
    return (size + (n - 1)) & ~(n - 1);
}

// PUBLIC

void kset_active_keyboard_listener(keyboard_event_t *event) {
    active_keyboard_event = event;
}

keyboard_event_t *kget_active_keyboard_listener(void) {
    return active_keyboard_event;
}

bool kpoll_event(keyboard_event_t *event) {
    if (kget_active_keyboard_listener() != event) {
        kset_active_keyboard_listener(event);
    }

    if (event->is_in) {
        event->is_in = false;
        return true;
    }

    return false;
}

void kvideo_required(void *backbuffer, void *framebuffer, uint32_t screen_width,
                     uint32_t screen_height, uint32_t pitch) {
    kvideo_driver->backbuffer = backbuffer;
    kvideo_driver->framebuffer = framebuffer;
    kvideo_driver->width = screen_width;
    kvideo_driver->height = screen_height;
    kvideo_driver->pitch = pitch;

    kvideo_driver->clear_screen = fb_clear_screen;
    kvideo_driver->draw_pixel = fb_draw_pixel;
    kvideo_driver->swap_buffers = fb_swap_buffers;
}

video_driver_t *kget_video_details(void) { return kvideo_driver; }

void kinit(void) {
    arch_enable_interupts();
    kdebug_printDONE_msg("Intrupts enabled");

    // keyboard event and the active keyboard listener
    keyboard_init();
    kdebug_printDONE_msg("Initilized keyboard driver");

    keyboard_event_init(active_keyboard_event);
    kset_active_keyboard_listener(active_keyboard_event);
    kdebug_printDONE_msg("Setted up keyboard event");
}

void kernel_panic(uint8_t mode, const char *title, interrupt_frames *frame) {
    typedef struct {
        const char *name;
        uint64_t value;
    } reg_entry_t;

    kdebug_logs_console_clear(kdebug_theme_get_cli_theme_bg(),
                              kdebug_theme_get_cli_theme_fg());

    if (mode == KERNEL_PANIC_MODE_DEBUG) {
        if (!title || !frame) {
            const char error_title[] =
                "\033[31mError from Kernel Panic thrower: \033[0m\n";
            kdebug_puts(error_title, sizeof(error_title));

            if (!title) {
                const char err[] =
                    "\nIn mode 0 (DEBUG) unexpected title NULL.\n";
                kdebug_puts(err, sizeof(err));
            }

            if (!frame) {
                const char err[] =
                    "\nIn mode 0 (DEBUG) unexpected interrupt frames NULL.\n";
                kdebug_puts(err, sizeof(err));
            }

            kdebug_puts("\n", 2);

            const char msg[] = "CPU Hanging...\nPlease restart your computer.";
            kdebug_puts(msg, sizeof(msg));

            arch_hang(true);
        }

        reg_entry_t registers[20] = {
            {"rax", frame->rax}, {"rbx", frame->rbx}, {"rcx", frame->rcx},
            {"rdx", frame->rdx}, {"rsi", frame->rsi}, {"rbp", frame->rbp},
            {"rdi", frame->rdi}, {"r8", frame->r8},   {"r9", frame->r9},
            {"r10", frame->r10}, {"r11", frame->r11}, {"r12", frame->r12},
            {"r13", frame->r13}, {"r14", frame->r14}, {"r15", frame->r15},
            {"rip", frame->rip}, {"cs", frame->cs},   {"rflags", frame->rflags},
            {"rsp", frame->rsp}, {"ss", frame->ss}};

        const uint8_t MAX_REG_IN_ONE_LINE = 2;

        kdebug_printKernelPanic_msg(title, true);
        kdebug_puts("\n\n\033[92mRegisters:\n\033[0m", 22);

        const char reset_color[] = "\033[0m";
        const char reg_color[] = "\033[95m";
        const char num_color[] = "\033[36m";

        for (size_t i = 0, j = 1; i < 20; ++i) {
            const char *reg_name = registers[i].name;
            size_t reg_name_size = libk_strlen(reg_name);

            char reg_value[25];
            size_t value_size = libk_u64_to_str(reg_value, registers[i].value);

            size_t buf_size = sizeof(reg_color) - 1     // color for reg name
                              + reg_name_size           // reg name
                              + sizeof(reset_color) - 1 // color reset
                              + 2                       // border
                              + sizeof(num_color) - 1   // color for reg value
                              + value_size              // value size of the reg
                              + sizeof(reset_color) - 1 // resetting color
                              + 2 // seperator between regs
                              + 1 // null terminator
                ;

            char full_details[buf_size];
            size_t ip = 0;

            // color for register name
            for (size_t k = 0; k < sizeof(reg_color) - 1; ++k) {
                full_details[ip++] = reg_color[k];
            }

            // register name
            for (size_t k = 0; k < reg_name_size; ++k) {
                full_details[ip++] = reg_name[k];
            }

            // resetting color
            for (size_t k = 0; k < sizeof(reset_color) - 1; ++k) {
                full_details[ip++] = reset_color[k];
            }

            // border between register name and value (size=2)
            full_details[ip++] = ':';
            full_details[ip++] = ' ';

            // color for register value
            for (size_t k = 0; k < sizeof(num_color) - 1; ++k) {
                full_details[ip++] = num_color[k];
            }

            // register value
            for (size_t k = 0; k < value_size; ++k) {
                full_details[ip++] = reg_value[k];
            }

            // resetting color
            for (size_t k = 0; k < sizeof(reset_color) - 1; ++k) {
                full_details[ip++] = reset_color[k];
            }

            // seperator between registers (size=2)
            full_details[ip++] = ',';
            full_details[ip++] = ' ';

            // null terminator
            full_details[ip++] = '\0';

            kdebug_puts(full_details, ip);

            if (j >= MAX_REG_IN_ONE_LINE) {
                kdebug_putc('\n');
                j = 1;
            } else {
                ++j;
            }
        }

        char ec_str[4];
        size_t ec_str_size = libk_u64_to_str(ec_str, frame->error_code);
        char ec_label[] = "\033[92mError Code:\033[0m ";

        size_t buf_size_ec_details = 1                          // newline
                                     + sizeof(ec_label)         // label of ec
                                     + sizeof(num_color) - 1    // ec color
                                     + ec_str_size              // for the ec
                                     + sizeof(reset_color) - 1; // reset color

        char full_ec_details[buf_size_ec_details];
        size_t ip_for_ec = 0;

        // newline
        full_ec_details[ip_for_ec++] = '\n';

        // label
        for (size_t i = 0; i < sizeof(ec_label); ++i) {
            full_ec_details[ip_for_ec++] = ec_label[i];
        }

        // ec color
        for (size_t i = 0; i < sizeof(num_color); ++i) {
            full_ec_details[ip_for_ec++] = num_color[i];
        }

        // ec
        for (size_t i = 0; i < ec_str_size; ++i) {
            full_ec_details[ip_for_ec++] = ec_str[i];
        }

        // reset color
        for (size_t i = 0; i < sizeof(reset_color); ++i) {
            full_ec_details[ip_for_ec++] = reset_color[i];
        }

        kdebug_puts(full_ec_details, ip_for_ec);

        const char conclusion[] =
            "\n\n\033[93mTurned off Interrupts. CPU "
            "Hanging.\n\n\033[0mPlease restart your computer.\033";
        kdebug_puts(conclusion, sizeof(conclusion));

        arch_hang(true);
    }

    else {
        // TODO: Make ts into normal string so that compiler will use memcpy().
        // (AFTER MAKING MEMCPY)
        static const char *const logo_ascii =
            "\n\n"
            "\033[91m      __                                               \n"
            "\033[91m       _ \\ \\                                         \n"
            "\033[91m      | | | |                                          \n"
            "\033[91m      | | | |                                          \n"
            "\033[91m      | |/_/    \033[31mKERNEL PANIC  :(               \n"
            "\033[91m      |_|\\ \\  \033[0m  Please restart your computer  \n"
            "\033[91m      | | | |                                          \n"
            "\033[91m      | | | |                                          \n"
            "\033[91m      | |/_/                                           \n"
            "\033[91m      |_|                                              \n"
            "\n\n\n"
            "\033[96m  Something unusual happened which even crashed the "
            "entire "
            "Kernel.\n  Please restart your computer.\n\n\033[31m  ";

        kdebug_puts(logo_ascii, libk_strlen(logo_ascii));

        if (title) {
            kdebug_puts(title, libk_strlen(title));
        }

        arch_hang(true);
    }
}

void kheap_init(void) {
    void *page = vmm_alloc_pages(1);
    if (!page) {
        kernel_panic(KERNEL_PANIC_MODE_NORMAL,
                     "Failed to allocate page for heap initilization", nullptr);
        return;
    }

    heap_head = (heap_block *)page;

    heap_head->size = PAGE_SIZE - sizeof(heap_block);
    heap_head->is_free = true;
    heap_head->magic = HEAP_MAGIC;
    heap_head->next = nullptr;
}

void *kmalloc(size_t n) {
    if (n == 0) {
        return nullptr;
    }

    n = align_up(16, n);

    heap_block *curr = heap_head;
    heap_block *prv = nullptr;

    // searching for a free block which is big enough
    while (curr) {
        if (curr->is_free && curr->size >= n) {
            // if block is bigger than necessary, splitting (alteast 16 bits)
            if (curr->size > n + sizeof(heap_block) + 16) {
                heap_block *new_block =
                    (heap_block *)((uint8_t *)curr + sizeof(heap_block) + n);

                new_block->size = curr->size - n - sizeof(heap_block);
                new_block->is_free = true;
                new_block->magic = HEAP_MAGIC;
                new_block->next = curr->next;

                curr->next = new_block;
                curr->size = n;
            }

            curr->is_free = false;
            return (void *)((uint8_t *)curr + sizeof(heap_block));
        }

        prv = curr;
        curr = curr->next;
    }

    // no existing block was big enough (creating new page from VMM)
    size_t total_size = n + sizeof(heap_block);
    size_t pages_needed = (total_size + PAGE_SIZE - 1) / PAGE_SIZE;

    void *new_mem = vmm_alloc_pages(pages_needed);
    if (!new_mem) {
        kernel_panic(KERNEL_PANIC_MODE_NORMAL, "Out of virtual memory",
                     nullptr);
        return nullptr;
    }

    heap_block *new_block = (heap_block *)new_mem;
    new_block->size = (pages_needed * PAGE_SIZE) - sizeof(heap_block);
    new_block->is_free = false;
    new_block->magic = HEAP_MAGIC;
    new_block->next = nullptr;

    if (prv) {
        prv->next = new_block;
    } else {
        heap_head = new_block;
    }

    return (void *)((uint8_t *)new_block + sizeof(heap_block));
}

void kfree(void *ptr) {
    if (!ptr) {
        return;
    }

    heap_block *block = (void *)((uint8_t *)ptr - sizeof(heap_block));
    if (block->magic != HEAP_MAGIC) {
        kernel_panic(KERNEL_PANIC_MODE_NORMAL,
                     "Heap Corruption detected on kfree", nullptr);
        return;
    }

    block->is_free = true;

    // merging multiple free blocks into one large ahh block
    {
        heap_block *curr = heap_head;
        while (curr) {
            if ((curr->is_free && curr->next && curr->next->is_free) &&
                (curr->next->magic == HEAP_MAGIC) &&
                // if the exact adderess of the following equals to the start of
                // next block, they are touching each other
                ((uint8_t *)curr + sizeof(heap_block) + curr->size ==
                 (uint8_t *)curr->next)) {
                curr->size += curr->next->size + sizeof(heap_block);
                curr->next = curr->next->next;
                continue;
            }

            curr = curr->next;
        }
    }

    // releasing pages back to VMM
    heap_block *curr = heap_head;
    heap_block *prev = NULL;
    while (curr) {
        uintptr_t block_start = (uintptr_t)curr;
        size_t total_bytes = sizeof(heap_block) + curr->size;

        if ((block_start & 0xFFF) == 0 && (total_bytes % PAGE_SIZE) == 0 &&
            curr->is_free && total_bytes >= PAGE_SIZE) {

            if (prev) {
                prev->next = curr->next;
            } else {
                heap_head = curr->next;
            }

            size_t pages = total_bytes / PAGE_SIZE;
            vmm_free_pages((void *)block_start, pages);

            curr = (prev) ? prev->next : heap_head;
        } else {
            prev = curr;
            curr = curr->next;
        }
    }
}

void *krealloc(void *ptr, size_t n) {
    if (!ptr || n == 0) {
        return nullptr;
    }

    heap_block *block = (void *)((uint8_t *)ptr - sizeof(heap_block));
    if (!block) {
        kernel_panic(KERNEL_PANIC_MODE_NORMAL, "Invalid heap block", nullptr);
        return nullptr;
    }

    if (block->magic != HEAP_MAGIC) {
        kernel_panic(KERNEL_PANIC_MODE_NORMAL,
                     "Heap block magic number not matching", nullptr);
        return nullptr;
    }

    if (block->size == n) {
        return ptr;
    }

    void *new_heap = kmalloc(n);
    if (!new_heap) {
        kernel_panic(KERNEL_PANIC_MODE_NORMAL,
                     "Unable to allcate new heap to reallocate", nullptr);
        return nullptr;
    }

    libk_memcpy(new_heap, ptr, block->size);
    kfree(ptr);

    return new_heap;
}

void kmain(void) {
    kdebug_printDONE_msg("Reached to to kmain");
    arch_hang(false);
}
