#ifndef VM_KMALLOC_H
#define VM_KMALLOC_H

#include <types.h>

/**
 * Allocates a chunk of memory, at least s bytes in size. Returns NULL if the
 * memory could not be made available.
 */
void *kmalloc(size_t s);

/**
 * Allocates a chunk of page allocated memory, s bytes in size, and stores the
 * physical address of the page in the specified memory, if not NULL. Returns
 * NULL if memory could not be allocated.
 */
void *kmalloc_ap(size_t s, uintptr_t *physical);

/**
 * Frees the specified chunk of memory. Pointer p must point to the beginning of
 * a previously-allocated region of memory.
 */
void kfree(void *p);

#endif