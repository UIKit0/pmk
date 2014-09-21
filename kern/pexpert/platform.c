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
}