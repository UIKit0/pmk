#include "vm.h"
#include "physical.h"

#include "pexpert/platform.h"

// take address to get kernel's end address
extern char __kern_end;

// gets the number of bytes on the dumb kernel heap
unsigned int kheap_dumb_get_bytes(void);

// kernel pages are mapped as RW
#define	VM_FLAGS_KERNEL kPlatformPageGlobal

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

	KDEBUG("Available RAM: %lu bytes\n", vm_state.mem_total);

	// initialise the physical manager
	vm_init_phys_allocator(vm_state.mem_total);

	// Initialise physical manager
	platform_pm_init();

	// get the blank kernel page directory
	vm_state.kernel_table = platform_pm_get_kernel_table();


	// reserve the kernel's memory
	uintptr_t kernel_load_addr = bootargs->load_address_phys;
	int kernel_size = ((((uintptr_t) &__kern_end) & 0x0FFFFFFF) - kernel_load_addr);

	kernel_size += kheap_dumb_get_bytes();
	//kernel_size += 1024 * 64;

	//KDEBUG("Kernel reserved 0x%X\n", kernel_size);

	vm_reserve_phys(kernel_size + 0x100000);

	// map the kernel memory
	for (int i = 0; i < kernel_size + 0x100000; i += 0x1000) {
		platform_pm_map(vm_state.kernel_table, i + 0xC0000000, i, VM_FLAGS_KERNEL);
	}

	// Allocate some memory for the kernel heap, pls.

	// switch pagetable
	platform_pm_switchto(vm_state.kernel_table);
	KDEBUG("Set up paging\n");
}

/**
 * Initialise the kernel's page tables. This basically means mapping the
 * kernel's data sections into the first 16M of the kernel map.
 */