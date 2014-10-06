#include "platform.h"

/**
 * Performs initialisation of the platform.
 */
void pexpert_init(void) {
	// Initialise the low-level components of the platform.
	platform_init();

	/*
	 * Initialise and parse the boot arguments. We do this before the console
	 * to determine what console mode should be used, as the system may be in
	 * graphical mode.
	 */
	platform_bootarg_parse();

	// determine which console to initialise
	const platform_bootargs_t *args = platform_bootarg_get();

	// With the platform initialised, initialise the console.
	if(!args->framebuffer.isVideo) {
		platform_console_txt_clear();
	}

	KINFO("Initialising platform \"%s\"\n", platform_name);

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