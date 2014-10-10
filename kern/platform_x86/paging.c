#include "x86.h"
#include "paging_types.h"

#include "vm/kmalloc.h"

#define	PAGE_SIZE 4096

// Kernel page directory in BSS
static __attribute__((__section__(".pagetable"))) page_directory_t x86_system_pagedir;

/**
 * Initialises the physical memory manager.
 */
void platform_pm_init(void) {
	/*
	 * Enable global addresses. This helps with minimising the TLB flush
	 * overhead when performing a context switch, as kernel pages can stay in
	 * the TLB.
	 */
	uint32_t cr4;
	__asm__ volatile("mov %%cr4, %0" : "=r" (cr4));
	cr4 |= (1 << 7);
	__asm__ volatile("mov %0, %%cr4" : : "r"(cr4));

	x86_system_pagedir.physAddr = (((uintptr_t) &x86_system_pagedir.tablesPhysical) - 0xC0000000);

	//KDEBUG("table 0x%X 0x%X\n", (unsigned int) &x86_system_pagedir, (unsigned int) x86_system_pagedir.physAddr);
}

/**
 * Returns the kernel pagetable. This is stored in the BSS section, as it is
 * very hard to allocate memory for a page table before a working paging setup
 * is in place.
 */
platform_pagetable_t platform_pm_get_kernel_table(void) {
	// set up the physical address
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
 * Maps a given virtual address range to a given physical address range.
 */
void platform_pm_map(platform_pagetable_t t_in, uintptr_t virt, uintptr_t phys,
					 platform_page_flags_t flags) {
	// is there a page table for the 4MB region this falls under?
	page_directory_t *d = (page_directory_t *) t_in;

	unsigned int block = virt / 0x400000;
	if(!d->tables[block]) {
		// allocate a table
		uintptr_t tmp;
		page_table_t *table = kmalloc_ap(sizeof(page_table_t), &tmp);

		d->tables[block] = table;
		d->tablesPhysical[block] = tmp | 0x00000005; // USER | PRESENT

		// ensure this table is clared
		memset(table, 0x00, sizeof(page_table_t));
	}

	// configure the pagetable entry
	page_table_t *table = d->tables[block];
	int table_entry = (virt & 0x3FFFFF) / 0x1000;

	// set up physical address
	table->pages[table_entry].frame = phys >> 12;

	// flags
	table->pages[table_entry].global = flags & kPlatformPageGlobal;
	table->pages[table_entry].cache = flags & kPlatformPageUncachable;
	table->pages[table_entry].writethrough = flags & kPlatformPageWritethrough;
	table->pages[table_entry].user = flags & kPlatformPageUser;
	table->pages[table_entry].rw = !(flags & kPlatformPageReadOnly);

	// reset some state (dirty, accessed)
	table->pages[table_entry].dirty = 0;
	table->pages[table_entry].accessed = 0;

	// assume the page is present in memory
	table->pages[table_entry].present = !(flags & kPlatformPageNotPresent);
}

/**
 * Unmaps the page, starting at virt, from the pagetable. Whatever physical
 * memory that backs them is not released.
 */
void platform_pm_unmap(platform_pagetable_t t_in, uintptr_t virt) {
	// is there a page table for the 4MB region this falls under?
	page_directory_t *d = (page_directory_t *) t_in;

	unsigned int block = virt / 0x400000;
	ASSERT(d->tables[block]);

	// configure the pagetable entry: not present, address 0
	page_table_t *table = d->tables[block];
	int table_entry = (virt & 0x3FFFFF) / 0x1000;

	table->pages[table_entry].frame = 0;
	table->pages[table_entry].present = 0;
}

/**
 * Translates a virtual address in a given pagetable to a physical address.
 */
uintptr_t platform_pm_virt_to_phys(platform_pagetable_t t_in, uintptr_t virt) {
	// find the page table that maps this 4M range
	page_directory_t *dir = (page_directory_t *) t_in;
	unsigned int dir_offset = virt / 0x400000;

	// is the table mapped, first of all?
	if(!dir->tablesPhysical[dir_offset]) {
		return 0;
	}

	// get the pagetable and check if this region is mapped
	page_table_t *table = dir->tables[dir_offset];
	unsigned int table_offset = (virt & 0x3FFFFF) / PAGE_SIZE;

	if(!table->pages[table_offset].present) {
		return 0;
	}

	// get the table's physical frame
	uintptr_t physical = table->pages[table_offset].frame << 12;
	physical += virt & ~(PAGE_SIZE - 1);

	return physical;
}

/**
 * Check whether a given page has been accessed, i.e. check the dirty bit. If
 * hardware does not support this, this function will return false.
 */
bool platform_pm_is_dirty(platform_pagetable_t t_in, uintptr_t virt) {
	// find the page table that maps this 4M range
	page_directory_t *dir = (page_directory_t *) t_in;
	unsigned int dir_offset = virt / 0x400000;

	// is the table mapped, first of all?
	if(!dir->tablesPhysical[dir_offset]) {
		return false;
	}

	// get the pagetable and check if this region is mapped
	page_table_t *table = dir->tables[dir_offset];
	unsigned int table_offset = (virt & 0x3FFFFF) / PAGE_SIZE;

	if(!table->pages[table_offset].present) {
		return false;
	}

	// get the dirty bit's status
	return table->pages[table_offset].dirty;
}

/**
 * Resets the dirty bit on a page. If the hardware does not support dirty bits,
 * nothing happens.
 */
void platform_pm_clear_dirty(platform_pagetable_t t_in, uintptr_t virt) {
	// find the page table that maps this 4M range
	page_directory_t *dir = (page_directory_t *) t_in;
	unsigned int dir_offset = virt / 0x400000;

	// is the table mapped, first of all?
	if(!dir->tablesPhysical[dir_offset]) {
		return;
	}

	// get the pagetable and check if this region is mapped
	page_table_t *table = dir->tables[dir_offset];
	unsigned int table_offset = (virt & 0x3FFFFF) / PAGE_SIZE;

	if(!table->pages[table_offset].present) {
		return;
	}

	// clear dirty bit
	table->pages[table_offset].dirty = false;
}

/**
 * Checks if a given address is valid in a page table. This can be evaluated 
 * for either user or kernel privileges.
 */
bool platform_pm_is_valid(platform_pagetable_t t_in, uintptr_t virt, bool user) {
	// find the page table that maps this 4M range
	page_directory_t *dir = (page_directory_t *) t_in;
	unsigned int dir_offset = virt / 0x400000;

	// is the table mapped, first of all?
	if(!dir->tablesPhysical[dir_offset]) {
		return false;
	}

	// get the pagetable and check if this region is mapped
	page_table_t *table = dir->tables[dir_offset];
	unsigned int table_offset = (virt & 0x3FFFFF) / PAGE_SIZE;

	// is the address valid?
	return user ? (table->pages[table_offset].present & table->pages[table_offset].user) : table->pages[table_offset].present;
}

/**
 * Switches to a given pagetable. This does not verify its contents beforehand:
 * it is the responsibility of the caller to do so.
 */
void platform_pm_switchto(platform_pagetable_t table) {
	page_directory_t *d = (page_directory_t *) table;
	unsigned int addr = d->physAddr;
	__asm__ volatile("mov %0, %%cr3" : : "r" (addr));
}

/**
 * Invalidates an address in the MMU's TLB, if applicable.
 */
void platform_pm_invalidate(void* m) {
    // add memory to clobber list: force optimiser off
    __asm__ volatile("invlpg %0" : : "m"(m) : "memory");
}

/**
 * An internal native pagefault handler that redirects to that of the VM
 * manager
 */
void x86_pagefault_handler(x86_registers_t reg) {
	uintptr_t faulting_address;
	__asm__ volatile("mov %%cr2, %0" : "=r" (faulting_address));

//	bool isUser = reg.err_code & 0x4;
//	bool isWrite = reg.err_code & 0x2;
//	bool isPresent = reg.err_code & 0x1;

	KDEBUG("Page fault! (error %u at 0x%X)\n", (unsigned int) reg.err_code, (unsigned int) faulting_address);
	KERROR("EAX: %08X EBX: %08X ECX: %08X EDX: %08X\n", (unsigned int) reg.eax, (unsigned int) reg.ebx, (unsigned int) reg.ecx, (unsigned int) reg.edx);
	KERROR("EIP: %08X  CS: %08X FLG: %08X USP: %08X\n", (unsigned int) reg.eip, (unsigned int) reg.cs, (unsigned int) reg.eflags, (unsigned int) reg.useresp);

	while(1);
}