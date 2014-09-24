#include "x86.h"
#include "interrupt.h"

// IDT
static idt_entry_t sys_idt[256];

// Internal private functions
static void x86_idt_set_gate(uint8_t entry, uint32_t function, uint8_t segment, uint8_t flags);
static void x86_idt_install(void* base, uint16_t size);

// Exception handlers
extern void x86_isr0(void);
extern void x86_isr1(void);
extern void x86_isr2(void);
extern void x86_isr3(void);
extern void x86_isr4(void);
extern void x86_isr5(void);
extern void x86_isr6(void);
extern void x86_isr7(void);
extern void x86_isr8(void);
extern void x86_isr9(void);
extern void x86_isr10(void);
extern void x86_isr11(void);
extern void x86_isr12(void);
extern void x86_isr13(void);
extern void x86_isr14(void);
extern void x86_isr15(void);
extern void x86_isr16(void);
extern void x86_isr17(void);
extern void x86_isr18(void);

// Dummy IRQ handler
extern void x86_irq_dummy(void);

/**
 * Sets up the interrupt vectors table for the processor. This will install any
 * handlers for CPU exceptions as well, and install stubs that call into the
 * kernel for interrupts.
 */
void platform_int_init(void) {
	idt_entry_t* idt = (idt_entry_t *) &sys_idt;

	// Clear IDT
	memclr(idt, sizeof(idt_entry_t) * 256);

	for(int i = 0; i < 256; i++) {
		x86_idt_set_gate(i, (uint32_t) x86_irq_dummy, GDT_KERNEL_CODE, 0x8E);
	}

	// Install exception handlers
	x86_idt_set_gate(0, (uint32_t) x86_isr0, GDT_KERNEL_CODE, 0x8E);
	x86_idt_set_gate(1, (uint32_t) x86_isr1, GDT_KERNEL_CODE, 0x8E);
	x86_idt_set_gate(2, (uint32_t) x86_isr2, GDT_KERNEL_CODE, 0x8E);
	x86_idt_set_gate(3, (uint32_t) x86_isr3, GDT_KERNEL_CODE, 0x8E);
	x86_idt_set_gate(4, (uint32_t) x86_isr4, GDT_KERNEL_CODE, 0x8E);
	x86_idt_set_gate(5, (uint32_t) x86_isr5, GDT_KERNEL_CODE, 0x8E);
	x86_idt_set_gate(6, (uint32_t) x86_isr6, GDT_KERNEL_CODE, 0x8E);
	x86_idt_set_gate(7, (uint32_t) x86_isr7, GDT_KERNEL_CODE, 0x8E);
	x86_idt_set_gate(8, (uint32_t) x86_isr8, GDT_KERNEL_CODE, 0x8E);
	x86_idt_set_gate(9, (uint32_t) x86_isr9, GDT_KERNEL_CODE, 0x8E);
	x86_idt_set_gate(10, (uint32_t) x86_isr10, GDT_KERNEL_CODE, 0x8E);
	x86_idt_set_gate(11, (uint32_t) x86_isr11, GDT_KERNEL_CODE, 0x8E);
	x86_idt_set_gate(12, (uint32_t) x86_isr12, GDT_KERNEL_CODE, 0x8E);
	x86_idt_set_gate(13, (uint32_t) x86_isr13, GDT_KERNEL_CODE, 0x8E);
	x86_idt_set_gate(14, (uint32_t) x86_isr14, GDT_KERNEL_CODE, 0x8E); // page fault
	x86_idt_set_gate(15, (uint32_t) x86_isr15, GDT_KERNEL_CODE, 0x8E);
	x86_idt_set_gate(16, (uint32_t) x86_isr16, GDT_KERNEL_CODE, 0x8E);
	x86_idt_set_gate(17, (uint32_t) x86_isr17, GDT_KERNEL_CODE, 0x8E);
	x86_idt_set_gate(18, (uint32_t) x86_isr18, GDT_KERNEL_CODE, 0x8E);

	// Install IDT (LIDT instruction)
	x86_idt_install((void *) idt, sizeof(idt_entry_t) * 256);

	KDEBUG("Installed x86 IDT\n");
}

/**
 * Returns the state of the processor's interrupt mask. True if interrupts are
 * enabled, false otherwise.
 */
bool platform_int_enabled(void) {
	uintptr_t flags;
	__asm__ volatile("pushf; pop %0" : "=g"(flags));
	return (bool) (flags & (1 << 9));
}

/**
 * Changes the interrupt mask state of the processor. Pass true to enable the
 * interrupts, or false to mask them. In essence, imagine the bool is logically
 * ANDed with interrupt lines to produce the CPU's IRQ signal.
 */
void platform_int_set_mask(bool m) {
	if(m) {
		__asm__ volatile("sti");
	} else {
		__asm__ volatile("cli");
	}
}

/*
 * Reloads the IDT, therefore flushing its caches
 */
void platform_int_update(void) {
	x86_idt_install((void *) &sys_idt, sizeof(idt_entry_t) * 256);
}

/*
 * Sets an IDT gate.
 */
static void x86_idt_set_gate(uint8_t entry, uint32_t function, uint8_t segment, uint8_t flags) {
	idt_entry_t *ptr = (idt_entry_t *) &sys_idt;

	ptr[entry].offset_1 = function & 0xFFFF;
	ptr[entry].offset_2 = (function >> 0x10) & 0xFFFF;
	ptr[entry].selector = segment;
	ptr[entry].flags = flags; // OR with 0x60 for user level
	ptr[entry].zero = 0x00;
}

/*
 * Loads the IDTR register.
 */
static void x86_idt_install(void* base, uint16_t size) {
	struct {
		uint16_t length;
		uint32_t base;
	} __attribute__((__packed__)) IDTR;
 
	IDTR.length = size;
	IDTR.base = (uint32_t) base;
	
	__asm__ volatile("lidt (%0)": : "p"(&IDTR));
}