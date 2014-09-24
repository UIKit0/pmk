#ifndef VM_VM_H
#define VM_VM_H

#include "pexpert/platform.h"

// kernel pages are mapped as RW
#define	VM_FLAGS_KERNEL kPlatformPageGlobal

/**
 * Initialises the virtual memory subsystem. This initialises internal state,
 * structures, and then builds a set of pagetables for the kernel.
 */
void vm_init(void);

/**
 * Returns the kernel's pagetable.
 */
platform_pagetable_t vm_get_pagetable(void);


#endif