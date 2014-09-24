#ifndef PLATFORM_X86_INTERRUPT
#define PLATFORM_X86_INTERRUPT

typedef struct x86_idt_descriptor {
	uint16_t offset_1;	// offset bits 0..15
	uint16_t selector;	// a code segment selector in GDT or LDT
	uint8_t zero;		// unused, set to 0
	uint8_t flags;		// type and attributes
	uint16_t offset_2;	// offset bits 16..31
} __attribute__((packed)) idt_entry_t;

#endif