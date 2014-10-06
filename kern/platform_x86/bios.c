#include "x86.h"
#include "bios.h"

// Address at which the real mode stub is loaded
#define	STUB_ADDR_PHYS			0x1000

// Address at which the stack is stored
#define	STUB_STACK_TOP_PHYS		0x3000
#define	STUB_STACK_BOTTOM_PHYS	0x4000

// Address at which data may be stored (stub is 0x2000/8K bytes)
#define	STUB_DATA_PHYS			0x4000 

// this is the assembly entry point
extern void x86_bios_entry();

/**
 * Calls the specified BIOS interrupt, with the specified registers at the time
 * of the function call. If requested, the register state after the call is
 * stored in a user-provided x86_bios_call_t structure.
 */
void x86_bios(uint8_t vector, x86_bios_call_t in, x86_bios_call_t *out) {
	x86_bios_stack_t *stack = (x86_bios_stack_t *) (STUB_STACK_BOTTOM_PHYS - sizeof(x86_bios_stack_t));

	// copy the register state
	memcpy(&stack->regs, &in, sizeof(x86_bios_call_t));
	stack->int_num = vector;

	// get the address of the code
	uintptr_t codeAddr = ((uintptr_t) &x86_bios_entry) & 0x00FFFFFF;

	// copy the block of code
	memcpy((void *) STUB_ADDR_PHYS, (void *) x86_bios_entry, codeAddr);

	// Jump to the stub code
	x86_bios_entry();
}

/**
 * Allocates a block of memory in lowmem that data can be put into. It places
 * the segment and offset into the variables whose address is inputted. Returns
 * the linear flat address.
 *
 * Note that this will usually return the same memory: it should not be relied
 * upon that the memory is consistent between invocations of x86_bios.
 */
uintptr_t x86_bios_alloc_mem(uint16_t *segment, uint16_t *offset) {
	// In our implementation, this is simply fixed.

	x86_bios_linear_to_far(STUB_DATA_PHYS, segment, offset);
	return STUB_DATA_PHYS;
}