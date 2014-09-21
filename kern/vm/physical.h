#ifndef VM_PHYSICAL_H
#define VM_PHYSICAL_H

#include <types.h>

/**
 * Initialises the physical memory manager, with the given number of physical
 * memory available.
 */
void vm_init_phys_allocator(uintptr_t bytes);

/**
 * Reserves all memory up to a specific physical address to the kernel. This is
 * used when the VM manager is first initialised, so pages belonging to kernel
 * data cannot be accidentally re-allocated.
 */
void vm_reserve_phys(intptr_t address);

/**
 * Allocates a single page of physical memory. Each page is 4K in size.
 */
intptr_t vm_allocate_phys(void);

#endif