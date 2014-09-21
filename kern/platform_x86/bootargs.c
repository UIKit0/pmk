#include "x86.h"
#include "multiboot.h"

// physical location of multiboot struct
void *x86_platform_multiboot_struct_addr;

// memory structure
static platform_bootargs_t bootargs;

/**
 * Parses the boot argument structure. This assumes that the bootup/init handler
 * fetched appropriate information.
 */
extern void platform_bootarg_parse(void) {
	multiboot_info_t *info = (multiboot_info_t *) x86_platform_multiboot_struct_addr;

	// available mem
	bootargs.total_mem = info->mem_upper;
	strncpy((char *) &bootargs.boot_params, (char *) info->cmdline, 512);

	// physical load address (fixed for x86 with multiboot)
	bootargs.load_address_phys = 0x00100000;
}

/**
 * Returns a pointer to the boot argument structure. It contains information on
 * various platform specifities, like what peripherals are available, and other
 * such things.
 */
const platform_bootargs_t *platform_bootarg_get(void) {
	return &bootargs;
}