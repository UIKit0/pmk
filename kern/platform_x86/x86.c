#include <types.h>
#include "x86.h"
#include "cpuid.h"

const char *platform_name = "x86-Based IBM Compatible";

/**
 * Initialises the platform's low-level features.
 */
void platform_init(void) {
/*	x86_cpu_t cpu = x86_detect_cpu();

	KINFO("Identiying CPU: ");
	KINFO("Manufacturer: %u (%s)\n", (unsigned int) cpu.manufacturer, (char *) &cpu.manufacturer_info.intel.brandString[0]);
	KINFO("Family: %i:%i\n", cpu.manufacturer_info.intel.family, cpu.manufacturer_info.intel.extendedFamily);*/
}

/**
 * x86 error handler
 */
void x86_error_handler(x86_registers_t reg) {
	KERROR("Exception %u\n", (unsigned int) reg.int_no);
	KERROR("%08X %08X %08X %08X\n", (unsigned int) reg.eax, (unsigned int) reg.ebx, (unsigned int) reg.ecx, (unsigned int) reg.edx);
	KERROR("%08X %08X %08X %08X\n", (unsigned int) reg.eip, (unsigned int) reg.cs, (unsigned int) reg.eflags, (unsigned int) reg.useresp);

	while(1);
}