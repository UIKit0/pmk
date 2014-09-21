#include "x86.h"
#include "paging_types.h"

#define	PAGE_SIZE 4096

// Kernel page directory in BSS
static page_directory_t x86_system_pagedir;

/**
 * Returns the kernel pagetable. This is stored in the BSS section, as it is
 * very hard to allocate memory for a page table before a working paging setup
 * is in place.
 */
platform_pagetable_t platform_pm_get_kernel_table(void) {
	return (platform_pagetable_t) &x86_system_pagedir;
}

/**
 * Creates a new pagetable, with no pages mapped. For example, on x86, this
 * creates the page directory only.
 */
platform_pagetable_t platform_pm_new(void) {
	// allocate physical memory for the page table. this needn't be page aligned
	return NULL;
}

/**
 * Maps a given virtual address range to a given physical address range, with
 * the specified number of pages mapped in this range.
 */
void platform_pm_map(platform_pagetable_t table, uintptr_t virt, uintptr_t phys, 
					 size_t pages, platform_page_flags_t flags);

/**
 * Unmaps num pages, starting at virt, from the pagetable. Whatever physical
 * memory that backs them is not released.
 */
void platform_pm_unmap(platform_pagetable_t table, uintptr_t virt, size_t pages);

/**
 * Translates a virtual address in a given pagetable to a physical address.
 */
uintptr_t platform_pm_virt_to_phys(platform_pagetable_t table, uintptr_t virt);

/**
 * Check whether a given page has been accessed, i.e. check the dirty bit. If
 * hardware does not support this, this function will return false.
 */
bool platform_pm_is_dirty(platform_pagetable_t table, uintptr_t virt);

/**
 * Resets the dirty bit on a page. If the hardware does not support dirty bits,
 * nothing happens.
 */
void platform_pm_clear_dirty(platform_pagetable_t table, uintptr_t virt);

/**
 * Checks if a given address is valid in a page table. This can be evaluated 
 * for either user or kernel privileges.
 */
bool platform_pm_is_valid(platform_pagetable_t table, uintptr_t virt, bool user);