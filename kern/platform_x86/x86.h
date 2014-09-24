/**
 * This header defines the initialisation functions for the x86 platform module
 *
 * It exposes some internal x86 functions that are used to interface with the
 * hardware, and should be included by the remaining components of this platform
 * module.
 */
#ifndef PLATFORM_X86_H
#define PLATFORM_X86_H

#include <types.h>
#include "pexpert/platform.h"

#include "msr.h"
#include "io.h"

#define GDT_KERNEL_CODE 0x08
#define GDT_KERNEL_DATA 0x10
#define GDT_USER_CODE 0x18
#define GDT_USER_DATA 0x20

// #define BREAKPOINT() __asm__ volatile("xchg	%bx, %bx");
/**
 * Reads the timestamp counter.
 */
static inline uint64_t x86_read_timestamp() {
	uint64_t ret;
	__asm__ volatile("rdtsc" : "=A"(ret));
	return ret;
}

typedef struct registers {
	uint32_t ds; // Data segment selector
	uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax; // Pushed by pusha.
	uint32_t int_no, err_code; // Interrupt number and error code (if applicable)
	uint32_t eip, cs, eflags, useresp, ss; // Pushed by the processor automatically.
} __attribute__((packed)) x86_registers_t;

#endif