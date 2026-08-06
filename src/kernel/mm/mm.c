#include <arch/irq.h>
#include <kernel/debug/debug_log/debug_log.h>
#include <kernel/kernel.h>
#include <kernel/mm/pmm.h>
#include <kernel/mm/vmm.h>
#include <libk/core.h>
#include <requests.h>

static uint64_t total_pages = 0;
static uint64_t used_pages = 0;
static uint8_t *bitmap = nullptr;

pml4_entry_t *pml4 = nullptr;

static vmem_node_t *vmem_free_list = nullptr;
static vmem_node_t vmem_nodes[VMEM_MAX_NODES];
static size_t vmem_nodes_used = 0;

// PRIVATE

// if table exist, returns it's virt addr and if not, make a new one and returns
// it's virt addr
static uint64_t *get_or_create_table(uint64_t *table, size_t index,
                                     uint64_t flags) {
    if (!table) {
        return nullptr;
    }

    struct limine_hhdm_response *hhdm = request_hhdm().response;
    if (!hhdm) {
        kdebug_printFAIL_msg("No response for HHDM Request.");
        arch_hang(true);
    }

    uint64_t entry = table[index];

    // if page exist and is present (last bit 1), returning itself
    if (entry & PAGE_PRESENT) {
        if (entry & PAGE_SIZE_FLAG) {
            kernel_panic(KERNEL_PANIC_MODE_NORMAL, "VMM hit a huge page!",
                         nullptr);
        }

        return (uint64_t *)((entry & ~0xFFF) + hhdm->offset);
    }

    // since dosen't exist, making one new
    uint64_t new_table_phy = (uint64_t)pmm_pgalloc();
    if (!new_table_phy) {
        return nullptr;
    }

    uint64_t *new_table_virt = (uint64_t *)(new_table_phy + hhdm->offset);
    libk_memset(new_table_virt, 0, PAGE_SIZE);
    table[index] = new_table_phy | flags | PAGE_PRESENT | PAGE_WRITABLE;

    return new_table_virt;
}

// PUBLIC

void pmm_bitmap_set(size_t bit) { bitmap[bit / 8] |= (1 << (bit % 8)); }

void pmm_bitmap_clear(size_t bit) { bitmap[bit / 8] &= ~(1 << (bit % 8)); }

bool pmm_bitmap_test(size_t bit) { return bitmap[bit / 8] & (1 << (bit % 8)); }

void pmm_init(void) {
    struct limine_memmap_response *memmap = request_memap().response;
    if (!memmap || memmap->entry_count <= 0) {
        kdebug_printFAIL_msg("No responce for Memory Map Request.");
        arch_hang(true);
    }

    struct limine_hhdm_response *hhdm = request_hhdm().response;
    if (!hhdm) {
        kdebug_printFAIL_msg("No responce for HHDM Request.");
        arch_hang(true);
    }

    // Finding the highest memory addr
    uint64_t max_entry = 0;
    for (size_t i = 0; i < memmap->entry_count; ++i) {
        struct limine_memmap_entry *entry = memmap->entries[i];

        uint64_t end = (entry->base + entry->length);
        if (end > max_entry) {
            max_entry = end;
        }
    }

    // Caluclating total pages (4kb each)
    total_pages = max_entry / 4096;
    size_t bitmap_size = (total_pages + 7) / 8;

    // Finding usable region to store bitmap
    uint64_t bitmap_addr = 0;
    for (size_t i = 0; i < memmap->entry_count; ++i) {
        struct limine_memmap_entry *entry = memmap->entries[i];

        if (entry->type == LIMINE_MEMMAP_USABLE &&
            entry->length >= bitmap_size) {
            bitmap_addr = entry->base;
            break;
        }
    }
    if (bitmap_addr == 0) {
        kdebug_printFAIL_msg(
            "Unable to find a region to store bitmap for PMM.");
        arch_hang(true);
    }

    // Allocating the bitmap
    bitmap = (uint8_t *)(bitmap_addr + hhdm->offset);

    // Marking all pages are used
    libk_memset(bitmap, 0xFF, bitmap_size);
    used_pages = total_pages;

    // Marking usable pages as free
    for (size_t i = 0; i < memmap->entry_count; ++i) {
        struct limine_memmap_entry *entry = memmap->entries[i];
        if (!entry) {
            kdebug_printFAIL_msg("Unable to get entry to make it free.");
            arch_hang(true);
        }

        if (entry->type != LIMINE_MEMMAP_USABLE) {
            continue;
        }

        uint64_t start = (entry->base + 4095) / 4096;
        uint64_t end = (entry->base + entry->length) / 4096;

        for (size_t j = start; j < end; ++j) {
            pmm_bitmap_clear(j);
            --used_pages;
        }
    }

    // Marking bitmap itself as used to prevent allocating itself
    uint64_t bitmap_start_page = bitmap_addr / 4096;
    uint64_t bitmap_end_page = (bitmap_addr + bitmap_size + 4095) / 4096;

    for (size_t j = bitmap_start_page; j < bitmap_end_page; ++j) {
        if (j < total_pages) {
            pmm_bitmap_set(j);
            ++used_pages;
        }
    }

    pmm_bitmap_set(0);
    ++used_pages;
}

void *pmm_pgalloc(void) {
    for (size_t i = 0; i < total_pages; ++i) {
        if (!pmm_bitmap_test(i)) {
            pmm_bitmap_set(i);
            ++used_pages;

            return (void *)(i * 4096);
        }
    }

    return NULL;
}

void pmm_pgfree(void *addr) {
    if (!addr) {
        return;
    }

    uint64_t page = (uint64_t)addr / 4096;
    if (page >= total_pages) {
        return;
    }

    if (pmm_bitmap_test(page)) {
        pmm_bitmap_clear(page);
        --used_pages;
    }
}

size_t pmm_get_free_pages(void) { return total_pages - used_pages; }

vmem_node_t *allocate_vmem_node(void) {
    if (vmem_nodes_used >= VMEM_MAX_NODES) {
        kernel_panic(KERNEL_PANIC_MODE_NORMAL, "VMM node pool exhausted",
                     nullptr);
    }

    return &vmem_nodes[vmem_nodes_used++];
}

void vmm_init(void) {
    // storing address of current PML4 (setted up by the bootloader)
    uint64_t cr3;
    __asm__ volatile("movq %%cr3, %0" : "=r"(cr3));

    // converting cr3 into virtual address using hhdm
    struct limine_hhdm_response *hhdm = request_hhdm().response;
    if (!hhdm) {
        kdebug_printFAIL_msg("No response for HHDM Request.");
        arch_hang(true);
    }

    // Stripping the 12 bits of cr3 cuz they contains caching and PCID flags,
    // not the address
    pml4 = (pml4_entry_t *)((cr3 & ~0xFFF) + hhdm->offset);

    vmem_free_list = allocate_vmem_node();
    vmem_free_list->vaddr_start = KERNEL_HEAP_START;
    vmem_free_list->pages = KERNEL_HEAP_PAGES;
    vmem_free_list->next = nullptr;
}

void vmm_map_page(void *phys, void *virt, uint64_t flags) {
    uint64_t vaddr = (uint64_t)virt;
    uint64_t paddr = (uint64_t)phys;

    /*
        Page table (64 bit) contains:

        Bits  |  Content
        ----  |  ----
        0-11  |  Flags (present, writable, user, etc.)
        12-51 |  Physical Address
        52-63 |  Reserved (or other CPU-specific flags)
    */

    uint16_t pml4_idx = (vaddr >> 39) & 0x1FF;
    uint16_t pdpt_idx = (vaddr >> 30) & 0x1FF;
    uint16_t pd_idx = (vaddr >> 21) & 0x1FF;
    uint16_t pt_idx = (vaddr >> 12) & 0x1FF;

    pdpt_entry_t *pdpt = get_or_create_table(pml4, pml4_idx, flags);
    if (!pdpt) {
        return;
    }

    pd_entry_t *pd = get_or_create_table(pdpt, pdpt_idx, flags);
    if (!pd) {
        return;
    }

    pt_entry_t *pt = get_or_create_table(pd, pd_idx, flags);
    if (!pt) {
        return;
    }

    pt[pt_idx] = paddr | flags | PAGE_PRESENT;

    // Invalidating virtual addres (flushing the virtual addr from cache of the
    // CPU)
    __asm__ volatile("invlpg %0" : : "m"(*(char *)virt));
}

void vmm_unmap_page(void *virt) {
    struct limine_hhdm_response *hhdm = request_hhdm().response;
    if (!hhdm) {
        kdebug_printFAIL_msg("No response for HHDM Request.");
        arch_hang(true);
    }

    uint64_t vaddr = (uint64_t)virt;

    uint16_t pml4_idx = (vaddr >> 39) & 0x1FF;
    uint16_t pdpt_idx = (vaddr >> 30) & 0x1FF;
    uint16_t pd_idx = (vaddr >> 21) & 0x1FF;
    uint16_t pt_idx = (vaddr >> 12) & 0x1FF;

    // getitng existing tables
    pdpt_entry_t *pdpt =
        (pdpt_entry_t *)((pml4[pml4_idx] & ~0xFFF) + hhdm->offset);
    if (!(pml4[pml4_idx] & PAGE_PRESENT)) {
        return;
    }

    pd_entry_t *pd = (pd_entry_t *)((pdpt[pdpt_idx] & ~0xFFF) + hhdm->offset);
    if (!(pdpt[pdpt_idx] & PAGE_PRESENT)) {
        return;
    }

    pt_entry_t *pt = (pt_entry_t *)((pd[pd_idx] & ~0xFFF) + hhdm->offset);
    if (!(pd[pd_idx] & PAGE_PRESENT)) {
        return;
    }

    pt[pt_idx] = 0;

    // Invalidating TLB
    __asm__ volatile("invlpg %0" : : "m"(*(char *)virt));
}

void *vmm_alloc_pages(size_t n) {
    if (n == 0) {
        return nullptr;
    }

    vmem_node_t *curr = vmem_free_list;
    vmem_node_t *prev = nullptr;

    while (curr) {
        if (curr->pages >= n) { // found a big enough region
            uint64_t start_vaddr = curr->vaddr_start;

            if (curr->pages == n) {
                // exact fit -- removing node from the free list
                if (prev) {
                    prev->next = curr->next;
                } else {
                    vmem_free_list = curr->next;
                }
            } else {
                // Splitting region -- moving start forward and reducing page
                // count
                curr->vaddr_start += n * PAGE_SIZE;
                curr->pages -= n;
            }

            for (size_t i = 0; i < n; ++i) {
                void *phy = pmm_pgalloc();
                if (!phy) {
                    kernel_panic(KERNEL_PANIC_MODE_NORMAL,
                                 "Out of physical memory in VMM", nullptr);
                }

                vmm_map_page(phy, (void *)(start_vaddr + i * PAGE_SIZE),
                             PAGE_PRESENT | PAGE_WRITABLE);
            }

            return (void *)start_vaddr;
        }

        prev = curr;
        curr = curr->next;
    }

    kernel_panic(KERNEL_PANIC_MODE_NORMAL, "Out of virtual memory!", nullptr);
    return nullptr;
}

void vmm_free_pages(void *vaddr, size_t count) {
    if (!vaddr || count == 0) {
        return;
    }

    struct limine_hhdm_response *hhdm = request_hhdm().response;
    if (!hhdm) {
        kdebug_printFAIL_msg("No response for HHDM Request.");
        arch_hang(true);
    }

    uint64_t addr = (uint64_t)vaddr;
    for (size_t i = 0; i < count; i++) {
        void *page_vaddr = (void *)(addr + i * PAGE_SIZE);

        uint64_t vaddr_val = (uint64_t)page_vaddr;
        uint16_t pml4_idx = (vaddr_val >> 39) & 0x1FF;
        uint16_t pdpt_idx = (vaddr_val >> 30) & 0x1FF;
        uint16_t pd_idx = (vaddr_val >> 21) & 0x1FF;
        uint16_t pt_idx = (vaddr_val >> 12) & 0x1FF;

        if (!(pml4[pml4_idx] & PAGE_PRESENT)) {
            continue;
        }
        pdpt_entry_t *pdpt =
            (pdpt_entry_t *)((pml4[pml4_idx] & ~0xFFF) + hhdm->offset);

        if (!(pdpt[pdpt_idx] & PAGE_PRESENT)) {
            continue;
        }
        pd_entry_t *pd =
            (pd_entry_t *)((pdpt[pdpt_idx] & ~0xFFF) + hhdm->offset);

        if (!(pd[pd_idx] & PAGE_PRESENT)) {
            continue;
        }
        pt_entry_t *pt = (pt_entry_t *)((pd[pd_idx] & ~0xFFF) + hhdm->offset);

        if (pt[pt_idx] & PAGE_PRESENT) {
            void *phys = (void *)(pt[pt_idx] & ~0xFFF);
            vmm_unmap_page(page_vaddr);
            pmm_pgfree(phys);
        }
    }

    vmem_node_t *new_node = allocate_vmem_node();

    new_node->vaddr_start = addr;
    new_node->pages = count;
    new_node->next = vmem_free_list;
    vmem_free_list = new_node;
}

pml4_entry_t *vmm_get_pml4(void) { return pml4; }
