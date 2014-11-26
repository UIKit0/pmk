#ifndef VM_VM_H
#define VM_VM_H

#include "pexpert/platform.h"

typedef enum {
	kVMAttributeUser = (1 << 0),
	kVMAttributeUncached = (kVMAttributeUser << 1),
	kVMAttributeReadOnly = (kVMAttributeUncached << 1),
	kVMAttributeWriteThru = (kVMAttributeReadOnly << 1),
} vm_attribute_t;

typedef struct {
	uintptr_t start;
	uintptr_t size;
	vm_attribute_t flags;
} vm_section_t;

/**
 * This is an array of the different sections in the VM manager that data can be
 * mapped into.
 */
static const vm_section_t vm_sections[] = {
	{0x00000000, 0xC0000000, kVMAttributeUser}, // userspace
	{0xC0000000, 0x02000000, 0}, // kernel .text/.data
	{0xC2000000, 0x01000000, kVMAttributeUncached}, // pagetable mapping area
	{0xC3000000, 0x01000000, kVMAttributeUncached}, // IPC mapping
	{0xC4000000, 0x04000000, 0}, // kernel drivers
	{0xC8000000, 0x28000000, 0}, // kernel heap
	{0xF0000000, 0x10000000, kVMAttributeUncached}, // MMIO

	{0, 0, 0} // list terminator
};

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