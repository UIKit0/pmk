#ifndef PLATFORM_PAGING_H
#define PLATFORM_PAGING_H

/**
 * Pagetables can have various flags associated with them. These bitfields can
 * be logically ORed together to specify the behaviour of a certain range of
 * pages.
 *
 * ReadOnly: This page cannot be written to. Doing so raises an exception.
 * Uncachable: This page will not be cached.
 * NoExecute: Attempting to execute code in this page raises an exception.
 * User: This page may be accessed by user code.
 * Global: 
 */
typedef unsigned int platform_page_flags_t;

enum {
	kPlatformPageReadOnly = (1 << 0),
	kPlatformPageUncachable = (1 << 1),
	kPlatformPageNoExecute = (1 << 2),
	kPlatformPageUser = (1 << 3),
	kPlatformPageGlobal = (1 << 4)
};

/**
 * An abstract type that references a physical pagetable. These are not defined
 * by the kernel's own VM system, but are instead handled by the physical mem
 * manager in the platform.
 *
 * This discourages direct modifications of the pagetables by the kernel code,
 * and exposes a well-defined API for manipulating page tables, hiding specific
 * implementation details from the kernel core.
 */
typedef void* platform_pagetable_t;

/**
 * Returns the kernel pagetable. This is stored in the BSS section, as it is
 * very hard to allocate memory for a page table before a working paging setup
 * is in place.
 */
platform_pagetable_t platform_pm_get_kernel_table(void);

/**
 * Creates a new pagetable, with no pages mapped. For example, on x86, this
 * creates the page directory only.
 */
platform_pagetable_t platform_pm_new(void);

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

#endif