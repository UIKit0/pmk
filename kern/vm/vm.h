#ifndef VM_VM_H
#define VM_VM_H

/**
 * Initialises the virtual memory subsystem. This initialises internal state,
 * structures, and then builds a set of pagetables for the kernel.
 */
void vm_init(void);

#endif