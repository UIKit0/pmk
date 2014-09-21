#include <types.h>

#include "pexpert/platform.h"
#include "vm/vm.h"

// Linker defines
extern uint32_t BUILD_NUMBER;
const char KERNEL_VERSION[] = "0.1";

/*
 * Kernel entry point
 */
void kmain(void) {
	// initialise the platform
	pexpert_init();

	KINFO("Panzer Microkernel %s: Copyright 2014 Tristan Seifert.\n", KERNEL_VERSION);

	// Initialise paging and VM subsystem
	vm_init();

	// Load any additional drivers from RAM disk

	// Initialise scheduler

	// Start driver and initialisation processes

	// Run scheduler
}