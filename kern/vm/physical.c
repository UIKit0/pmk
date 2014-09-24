#include "vm.h"
#include "physical.h"

#include "kmalloc.h"

/**
 * Physical memory manager. This keeps track of what physical memory pages have
 * been allocated, to whom they are allocated, and what their current state is.
 */

// Page size is determined by hardware, but all platforms support 4K pages.
#define	PAGE_SIZE 0x1000

// A bitmap of 4K frames of physical memory that are available.
static unsigned int* frames;
static unsigned int nframes;

// Macros used in the bitset algorithms.
#define INDEX_FROM_BIT(a) (a/(8*4))
#define OFFSET_FROM_BIT(a) (a%(8*4))

/**
 * Set a bit in the frames bitset
 *
 * @param frame_addr Physical memory address
 */
static void set_frame(uintptr_t frame_addr) {
	unsigned int frame = frame_addr / PAGE_SIZE;
	unsigned int idx = INDEX_FROM_BIT(frame);
	unsigned int off = OFFSET_FROM_BIT(frame);
	frames[idx] |= (0x1 << off);
}

/**
 * Clear a bit in the frames bitset
 *
 * @param frame_addr Physical memory address
 */
/*static void clear_frame(uintptr_t frame_addr) {
	unsigned int frame = frame_addr / PAGE_SIZE;
	unsigned int idx = INDEX_FROM_BIT(frame);
	unsigned int off = OFFSET_FROM_BIT(frame);
	frames[idx] &= ~(0x1 << off);
}*/

/**
 * Check if a certain page is allocated.
 *
 * @param frame_addr Physical memory address
 */
/*static bool test_frame(uintptr_t frame_addr) {
	unsigned int frame = frame_addr / PAGE_SIZE;
	unsigned int idx = INDEX_FROM_BIT(frame);
	unsigned int off = OFFSET_FROM_BIT(frame);
	return (frames[idx] & (0x1 << off));
}*/

/**
 * Find the first free frame that can be allocated
 */
static uintptr_t find_free_frame() {
	unsigned int i, j;
	for(i = 0; i < INDEX_FROM_BIT(nframes); i++) {
		if(frames[i] != 0xFFFFFFFF) { // nothing free, check next
			// At least one bit is free here
			for (j = 0; j < 32; j++) {
				unsigned int toTest = 0x1 << j;

				// If this frame is free, return
				if (!(frames[i] & toTest)) {
					return i*4*8+j;
				}
			}
		}
	}

	return -1;
}

/**
 * Initialises the physical memory manager, with the given number of physical
 * memory available.
 *
 * This allocates memory for the physical memory bitmap, which is used to alloc
 * physical memory when needed.
 */
void vm_init_phys_allocator(uintptr_t bytes) {
	// This page set allocates for lowmem, but this is reserved by the kernel
	unsigned int mem_end_page = bytes;
	nframes = mem_end_page / PAGE_SIZE;
	nframes += 0x100;

	// Allocate page frame table and clear it
	frames = (unsigned int *) kmalloc(INDEX_FROM_BIT(nframes));
	memclr(frames, INDEX_FROM_BIT(nframes));
}

/**
 * Allocates a single page of physical memory. Each page is 4K in size.
 */
uintptr_t vm_allocate_phys(void) {
	uintptr_t page = find_free_frame();
	page *= PAGE_SIZE;
	return page;
}

/**
 * Releases physical memory back to the system so it can be reallocated.
 */
void vm_deallocate_phys(uintptr_t address) {
	
}

/**
 * Reserves all memory up to a specific physical address to the kernel. This is
 * used when the VM manager is first initialised, so pages belonging to kernel
 * data cannot be accidentally re-allocated.
 */
void vm_reserve_phys(uintptr_t address) {
	// even out address to multiples of a page size
	if(address & (PAGE_SIZE - 1)) {
		address += PAGE_SIZE;
	}

	address &= ~(PAGE_SIZE - 1);

	for(intptr_t i = 0; i < address; i += PAGE_SIZE) {
		set_frame(i);
	}
}