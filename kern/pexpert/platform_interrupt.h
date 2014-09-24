#ifndef PLATFORM_INTERRUPT_H
#define PLATFORM_INTERRUPT_H

/**
 * Sets up the interrupt vectors table for the processor. This will install any
 * handlers for CPU exceptions as well, and install stubs that call into the
 * kernel for interrupts.
 */
extern void platform_int_init(void);

/**
 * Returns the state of the processor's interrupt mask. True if interrupts are
 * enabled, false otherwise.
 */
extern bool platform_int_enabled(void);

/**
 * Changes the interrupt mask state of the processor. Pass true to enable the
 * interrupts, or false to mask them. In essence, imagine the bool is logically
 * ANDed with interrupt lines to produce the CPU's IRQ signal.
 */
extern void platform_int_set_mask(bool m);

/**
 * Updates the interrupt table, if applicable.
 */
extern void platform_int_update(void);

#endif