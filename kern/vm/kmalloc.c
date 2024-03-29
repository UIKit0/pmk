#include "kmalloc.h"

extern char __kern_end;

extern void *kheap_smart_alloc(size_t size, bool aligned, uintptr_t *phys);

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
		return kheap_smart_alloc(s, false, NULL);
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

/*
 * Allocates a chunk of memory and gets physical address.
 *
 * @param sz Size of memory to allocate
 * @param phys Pointer to memory to place physical address in
 */
void *kmalloc_p(size_t s, uintptr_t *physical) {
	if(likely(state.use_smart_mapper)) {
		return kheap_smart_alloc(s, false, physical);		
	} else {
		// is the start placement address configured?
		if(!state.s.dumb.start_placement) {
			state.s.dumb.start_placement = (uintptr_t) &__kern_end;
		}

		// align size on 16-byte boundaries
		if(s & 0x0F) {
			s &= 0xFFFFFFF0;
			s += 0x10;
		}

		// get address and increment
		uintptr_t address = state.s.dumb.start_placement;

		state.s.dumb.start_placement += s;
		state.s.dumb.bytes_allocated += s;

		// convert to physical
		if(physical) {
			*physical = address - 0xC0000000;
		}

		return (void *) address;		
	}
}

/*
 * Allocates a page-aligned chunk of memory.
 *
 * @param sz Size of memory to allocate
 */
void *kmalloc_a(size_t sz) {
	return kmalloc_ap(sz, NULL);
}

/**
 * Allocates a chunk of page allocated memory, s bytes in size, and stores the
 * physical address of the page in the specified memory, if not NULL. Returns
 * NULL if memory could not be allocated.
 */
void *kmalloc_ap(size_t s, uintptr_t *physical) {
	if(likely(state.use_smart_mapper)) {
		return kheap_smart_alloc(s, true, physical);
	} else {
		// is the start placement address configured?
		if(!state.s.dumb.start_placement) {
			state.s.dumb.start_placement = (uintptr_t) &__kern_end;
		}

		// align size on 16-byte boundaries
		if(s & 0x0F) {
			s &= 0xFFFFFFF0;
			s += 0x10;
		}

		// is placement already page aligned?
		if(!(state.s.dumb.start_placement & 0xFFF)) {

		} else {
			// align to page bounds
			state.s.dumb.start_placement += 0x1000 - (state.s.dumb.start_placement & 0xFFF);
		}

		// get address and increment
		uintptr_t address = state.s.dumb.start_placement;

		state.s.dumb.start_placement += s;
		state.s.dumb.bytes_allocated += s;

		// convert to physical
		if(physical) {
			*physical = address - 0xC0000000;
		}

		return (void *) address;
	}

	return NULL;
}

/**
 * Returns the number of bytes on the dumb kernel heap.
 */
unsigned int kheap_dumb_get_bytes(void) {
	return state.s.dumb.bytes_allocated;
}