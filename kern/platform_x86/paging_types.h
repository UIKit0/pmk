#ifndef X86_PAGING_H
#define X86_PAGING_H

#include <types.h>

/**
 * An entry in a page table. This is in the format that the x86 hardware will
 * expect it in.
 */
typedef struct page {
	int present:1;		// Page present in memory
	int rw:1;			// Read-only if clear, readwrite if set
	int user:1;			// Supervisor level only if clear
	int writethrough:1; // When set, writethrough caching is enabled
	int cache:1;		// Disables caching of the page when set
	int accessed:1;		// Has the page been accessed since last refresh?
	int dirty:1;		// Has the page been written to since last refresh?
	int unused:1;		// Ignored bits
	int global:1;		// When set, not evicted from TLB on pagetable switch
	int unused2:3;		// More ignored bits
	int frame:20;		// Frame address (shifted right 12 bits)
} page_t;

/**
 * An x86 page table, containing space for 1024 pages. Each page maps to 4K of
 * memory, so a page table maps 4M of memory.
 */
typedef struct page_table {
	page_t pages[1024];
} page_table_t;

/**
 * This structure represents a page table for the x86 architecture. It contains
 * the page directory, as well as some metadata to keep track of the table's
 * whereabouts in physical memory.
 *
 * Note that the first entry in tablesPhysical is not actually a physical
 * pointer to a page table, but rather, the physical location of the array.
 */
typedef struct page_directory {
	// Array of pointers to page_table structs.
	page_table_t* tables[1024];
	// Physiocal address of tables pointed to by above. See for disclaimer	
	uintptr_t tablesPhysical[1024];
} page_directory_t;

#endif