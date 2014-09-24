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

	KINFO("PMK %s (build %u): Copyright 2014 Tristan Seifert.\n", KERNEL_VERSION, (unsigned int) &BUILD_NUMBER);

	// Initialise paging and VM subsystem
	vm_init();

	// Load any additional drivers from RAM disk
//	unsigned int d = 0xdead;
//	unsigned int b = d / 0;
//	KDEBUG("%u", b);

	uint32_t *cube = (uint32_t *) 0xDEADBEEF;
	KINFO("potato: %u\n", (unsigned int) *cube);

	// Initialise scheduler

	// Start driver and initialisation processes

	// Run scheduler
}