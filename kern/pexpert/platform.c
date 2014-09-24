#include "platform.h"

/**
 * Performs initialisation of the platform.
 */
void pexpert_init(void) {
	// Initialise the low-level components of the platform.
	platform_init();

	// With the platform initialised, initialise the console.
	platform_console_clear();
	KINFO("Initialising platform \"%s\"\n", platform_name);

	// Initialise boot args
	platform_bootarg_parse();

	// Install interrupt handlers
	platform_int_init();
}

/**
 * Causes a kernel panic.
 */
void pexpert_panic(const char *file, const int line, const char *message) {
	KERROR("Kernel panic!\n%s:%i %s\n", file, line, message);
}

/**
 * Platform IRQ handler: invoked by platform-specific stubs to handle an irq.
 */
void platform_irq_handler(uint32_t irq) {
	
}