#include "kmalloc.h"

extern char __kern_end;

/**
 * Overall state of the memory allocator. This encapsulates the state of both
 * the smart and dumb mappers: however, only one is ever used.
 */
static struct {
	bool use_smart_mapper;

	union {
		struct {
			uintptr_t start_placement;
			unsigned int bytes_allocated;
		} dumb;

		struct {

		} smart;
	} s;
} state;

/**
 * Allocates a chunk of memory, at least s bytes in size. Returns NULL if the
 * memory could not be made available.
 */
void *kmalloc(size_t s) {
	if(likely(state.use_smart_mapper)) {

	} else {
		// is the start placement address configured?
		if(!state.s.dumb.start_placement) {
			state.s.dumb.start_placement = (uintptr_t) &__kern_end;
		}

		// align s on 16-byte boundaries
		if(s & 0x0F) {
			s &= 0xFFFFFFF0;
			s += 0x10;
		}

		// get address and increment
		uintptr_t address = state.s.dumb.start_placement;
		
		state.s.dumb.start_placement += s;
		state.s.dumb.bytes_allocated += s;

		return (void *) address;
	}

	return NULL;
}

/**
 * Frees the specified chunk of memory. Pointer p must point to the beginning of
 * a previously-allocated region of memory.
 */
void kfree(void *p) {
	if(likely(state.use_smart_mapper)) {

	} else {

	}
}

/**
 * Returns the number of bytes on the dumb kernel heap.
 */
unsigned int kheap_dumb_get_bytes(void) {
	return state.s.dumb.bytes_allocated;
}