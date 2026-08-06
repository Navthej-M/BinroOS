#ifndef _VMM_H
#define _VMM_H

#include <libk/types.h>

#define PAGE_SIZE 4096

/// @name Page table entry flags
/// @{
#define PAGE_PRESENT (1 << 0)
#define PAGE_WRITABLE (1 << 1)
#define PAGE_USER (1 << 2)
#define PAGE_HUGE (1 << 7)
#define PAGE_SIZE_FLAG 0x80
/// @}

/// @name Layouts
/// @{
#define KERNEL_HEAP_START 0xFFFFFFFFC0000000
#define KERNEL_HEAP_END 0xFFFFFFFFDFFFFFFF
#define MMIO_START 0xFFFFFFFFE0000000
#define KERNEL_HEAP_PAGES 524288 // 2GB worth of 4KB pages
/// @}

/// @brief Maximum nodes available to allocate
#define VMEM_MAX_NODES 1024 // 1024 * 24 bytes = 24kb, safe!

/// @name Page table types
/// @{
typedef uint64_t pml4_entry_t;
typedef uint64_t pdpt_entry_t;
typedef uint64_t pd_entry_t;
typedef uint64_t pt_entry_t;
///@}

/// @brief Virtual Memory Node
typedef struct vmem_node {
    uint64_t vaddr_start;
    size_t pages;
    struct vmem_node *next;
} vmem_node_t;

/// @brief Allocate new vmem_node
vmem_node_t *allocate_vmem_node(void);

/// @brief Initialize VMM
void vmm_init(void);

/// @brief Map a physical page to a virtual address
///
/// @param phys Physical Address
/// @param virt Virtual Address
/// @param flags
void vmm_map_page(void *phys, void *virt, uint64_t flags);

/// @brief Unmap a virtual address
///
/// @param virt Virtual Address
void vmm_unmap_page(void *virt);

/// @brief Allocare n virtual page (with PMM backing)
///
/// @return Allocated virtual memory address
void *vmm_alloc_pages(size_t n);

/// @brief Free n virtual pages
///
/// @param virt Virtual Address
void vmm_free_pages(void *virt, size_t n);

/// @brief Get the current PML4 table
///
/// @return current PML4 table
pml4_entry_t *vmm_get_pml4(void);

#endif
