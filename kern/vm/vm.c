#include "vm.h"
#include "physical.h"

#include "kheap.h"

// take address to get kernel's end address
extern char __kern_end;

// gets the number of bytes on the dumb kernel heap
unsigned int kheap_dumb_get_bytes(void);

/**
 * Overall state of the Virtual Memory manager
 */
static struct {
	// Total memory installed in the system, past 0x100000, in bytes.
	uintptr_t mem_total;
	// Total memory used through mapped pagetables, in bytes
	uintptr_t mem_used;

	// Pagetable used for the kernel. Only has 0xC00000000-0xFFFFFFFF.
	platform_pagetable_t kernel_table;
} vm_state;

/**
 * Initialises the virtual memory subsystem. This initialises internal state,
 * structures, and then builds a set of pagetables for the kernel.
 */
void vm_init(void) {
	const platform_bootargs_t *bootargs = platform_bootarg_get();

	// initialise VM state
	vm_state.mem_total = bootargs->total_mem * 1024;
	vm_state.mem_used = 0;

	// initialise the physical manager
	vm_init_phys_allocator(vm_state.mem_total);

	// Initialise platform physical mappings manager
	platform_pm_init();

	// get the blank kernel page directory
	vm_state.kernel_table = platform_pm_get_kernel_table();

	// reserve the kernel's memory
	uintptr_t kernel_load_addr = bootargs->load_address_phys;
	int kernel_size = ((((uintptr_t) &__kern_end) & 0x0FFFFFFF) - kernel_load_addr);

	kernel_size += kheap_dumb_get_bytes();
	kernel_size += 1024 * 64;

	//KDEBUG("Kernel reserved 0x%X\n", kernel_size);

	vm_reserve_phys(kernel_size + 0x100000);

	// map the kernel memory
	for (int i = 0; i < kernel_size + 0x100000; i += 0x1000) {
		platform_pm_map(vm_state.kernel_table, i + VM_KERNEL_BASE, i, VM_FLAGS_KERNEL);
	}

	// Perform identity mapping for the real-mode accessible RAM (to 0x10FFEF)
	for (int i = 0; i < 0x110000; i += 0x1000) {
		platform_pm_map(vm_state.kernel_table, i, i, VM_FLAGS_KERNEL);
	}

	// Allocate some memory for the kernel heap and enable it
	for(int i = VM_KERNEL_HEAP_BASE; i < (VM_KERNEL_HEAP_BASE + 0x10000); i += 0x1000) {
		uintptr_t phys_addr = vm_allocate_phys();
		platform_pm_map(vm_state.kernel_table, i, phys_addr, VM_FLAGS_KERNEL);
	}

	// Map the video framebuffer
	if(bootargs->framebuffer.isVideo) {
		platform_console_vid_map();
	}

	kheap_install();

	// switch pagetable
	platform_pm_switchto(vm_state.kernel_table);

	// set up the video console
	if(bootargs->framebuffer.isVideo) {
		platform_console_vid_clear();
	}

/*	// test allocator
	uintptr_t a = (uintptr_t) kmalloc(0x1000);
	uintptr_t b = (uintptr_t) kmalloc(0x10000);
	KINFO("%X %X\n", (unsigned int) a, (unsigned int) b);

	kfree((void *) b);

	uintptr_t c = (uintptr_t) kmalloc(0x2320);
	uintptr_t d = (uintptr_t) kmalloc(0x20);
	KINFO("%X %X\n", (unsigned int) c, (unsigned int) d);*/

	KINFO("Available RAM: %luK\n", vm_state.mem_total / 1024);
}

/**
 * Initialise the kernel's page tables. This basically means mapping the
 * kernel's data sections into the first 16M of the kernel map.
 */

/**
 * Returns the kernel's pagetable.
 */
platform_pagetable_t vm_get_pagetable(void) {
	return vm_state.kernel_table;
}