#ifndef PLATFORM_PAGING_H
#define PLATFORM_PAGING_H

/**
 * Pagetables can have various flags associated with them. These bitfields can
 * be logically ORed together to specify the behaviour of a certain range of
 * pages.
 *
 * ReadOnly: This page cannot be written to. Doing so raises an exception.
 * Uncachable: This page will not be cached.
 * Writethrough: Use writethrough caching for this page, if possible.
 * NoExecute: Attempting to execute code in this page raises an exception.
 * User: This page may be accessed by user code.
 * Global: Page will not be evicted from TLB when pagetable switches.
 * NotPresent: Page raises a page fault when accessed.
 */
typedef unsigned int platform_page_flags_t;

enum {
	kPlatformPageReadOnly = (1 << 0),
	kPlatformPageUncachable = (1 << 1),
	kPlatformPageWritethrough = (1 << 2),
	kPlatformPageNoExecute = (1 << 3),
	kPlatformPageUser = (1 << 4),
	kPlatformPageGlobal = (1 << 5),
	kPlatformPageNotPresent = (1 << 6),
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
 * Initialises the physical memory manager.
 */
void platform_pm_init(void);

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
 * Maps a given virtual address range to a given physical address range.
 */
void platform_pm_map(platform_pagetable_t table, uintptr_t virt, uintptr_t phys, 
					 platform_page_flags_t flags);

/**
 * Unmaps the page, starting at virt, from the pagetable. Whatever physical
 * memory that backs them is not released.
 */
void platform_pm_unmap(platform_pagetable_t table, uintptr_t virt);

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

/**
 * Switches to a given pagetable. This does not verify its contents beforehand:
 * it is the responsibility of the caller to do so.
 */
void platform_pm_switchto(platform_pagetable_t table);

#endif