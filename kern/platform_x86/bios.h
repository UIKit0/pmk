/**
 * Provides an interface for making BIOS calls from protected mode.
 *
 * This temporarily disables protected mode and paging, switches the CPU into
 * real mode, performs the BIOS call, and stores the BIOS function result in
 * low memory.
 *
 * This exposes a nice C interface to the real mode calls: the secret sauce is
 * in the realmode.s file, which performs the realmode switch, BIOS call, and
 * resumes execution of the kernel.
 *
 * NOTE: This is incredibly slow, and should not be used unless there is no
 * alternative.
 */
#ifndef PLATFORM_X86_BIOS
#define PLATFORM_X86_BIOS

// Register state that is restored before the BIOS call, and saved after
typedef struct {
	uint32_t es, fs, gs;

	uint32_t edi, esi, ebp, esp;
	uint32_t ebx, edx, ecx, eax;
} __packed x86_bios_call_t;

// The stack frame we place on the stack for the real mode stub
typedef struct {
	uint32_t int_num;

	x86_bios_call_t regs;
} __packed x86_bios_stack_t;

/**
 * Calls the specified BIOS interrupt, with the specified registers at the time
 * of the function call. If requested, the register state after the call is
 * stored in a user-provided x86_bios_call_t structure.
 */
void x86_bios(uint8_t vector, x86_bios_call_t in, x86_bios_call_t *out);

/**
 * Allocates a block of memory in lowmem that data can be put into. It places
 * the segment and offset into the variables whose address is inputted. Returns
 * the linear flat address.
 *
 * Note that this will usually return the same memory: it should not be relied
 * upon that the memory is consistent between invocations of x86_bios.
 */
uintptr_t x86_bios_alloc_mem(uint16_t *segment, uint16_t *offset);

/**
 * Converts a far pointer (segment:offset) to a linear address.
 */
//static uintptr_t x86_bios_far_to_linear(uint16_t segment, uint16_t offset) {
//	return (segment << 4) + offset;
//}

/**
 * Converts a linear address to a far pointer (segment:offset.)
 *
 * 0x00D000 -> 0x0000:0xD000
 * 0x0B8000 -> 0xB000:0x8000
 */
static void x86_bios_linear_to_far(uintptr_t linear, uint16_t *segment, uint16_t *offset) {
	if(offset) {
		*offset = (linear & 0xFFFF);
	}

	if(segment) {
		*segment = (linear & 0x0F0000) >> 4;
	}
}

#endif