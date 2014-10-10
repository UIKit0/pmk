#include <types.h>

#include "pexpert/platform.h"
#include "vm/vm.h"
#include "scheduler/scheduler.h"

// Linker defines
extern uint32_t BUILD_NUMBER;
const char KERNEL_VERSION[] = "0.1";

/*
 * Kernel entry point
 */
void kmain(void) {
	// initialise the platform
	pexpert_init();

	// Initialise paging and VM subsystem
	vm_init();

	// print some info
	KINFO("PMK %s (build %u): Copyright 2014 Tristan Seifert <t@tseifert.me>. All rights reserved.\n", KERNEL_VERSION, (unsigned int) &BUILD_NUMBER);
	KDEBUG("Loading IO services from RAM disk...\n");

	// Load any additional drivers from RAM disk

	// Initialise scheduler
	scheduler_init();

	// Start driver and initialisation processes

	// Run scheduler
}