/* The bootloader will look at this image and start execution at the symbol
   designated as the entry point. */
ENTRY(_osentry)
OUTPUT_FORMAT(elf32-i386)

/* Tell where the various sections of the object files will be put in the final
   kernel image. */
SECTIONS {
	. = 0xC0100000;

	__kern_code_start = .;
	.text ALIGN(4K) : AT(ADDR(.text) - 0xC0000000) {
		*(.multiboot);
		*(.entry);
		*(.text);
		*(.rodata);
		*(.rodata*);
	}

/*	.modules : AT(ADDR(.modules) - 0xC0000000) {
		__kern_initcalls = .;
		*(.initcall0*);
		*(.initcall1*);
		*(.initcall2*);
		*(.initcall3*);
		*(.initcall4*);
		*(.initcall5*);
		*(.initcall6*);
		*(.initcall7*);
		BYTE(0x00); BYTE(0x00); BYTE(0x00); BYTE(0x00);

		__kern_lateinitcalls = .;
		*(.initcall8*);
		*(.initcall9*);
		BYTE(0x00); BYTE(0x00); BYTE(0x00); BYTE(0x00);

		__kern_exitcalls = .;

		*(.exitcall*);
		BYTE(0x00); BYTE(0x00); BYTE(0x00); BYTE(0x00);

		__kern_callsend = .;
	}*/
}

/*
 * Read/write data
 */
SECTIONS {
	/* Data segment */
	__kern_data_start = .;

	.data ALIGN(4K) : AT(ADDR(.data) - 0xC0000000) {
		/* Initialisers */
		*(.init*);
		*(.fini*);

		/* C++ destructor/constructor info */
		*(.ctors*);
		*(.dtors*);

		*(.jcr*);

		*(*.data);
	}

	__kern_data_size = SIZEOF(.data);

	/* Read-write data (uninitialized) and stack */
	__kern_bss_start = .;

	.bss ALIGN(4K) : AT(ADDR(.bss) - 0xC0000000) {
		*(.bss);
		*(COMMON);
	}

	/* The initial pagetable goes in its own 16K aligned section */
	.pagetable ALIGN(16K) : AT(ADDR(.pagetable) - 0xC0000000) {
		*(.pagetable);
	}

	__kern_bss_size = SIZEOF(.bss);
	__kern_size = __kern_bss_start - __kern_code_start;

	__kern_end = .;
}