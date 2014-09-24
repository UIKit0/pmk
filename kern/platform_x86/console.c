#include "x86.h"
#include "font_bold.h"

// Current X/Y position
static int current_x, current_y;

// Attributes to use in writing to the display
static uint8_t attrib = 0x07;

// Size of the console
#define	CONSOLE_WIDTH 80
#define	CONSOLE_HEIGHT 25

// Custom font loading
static void vga_load_font(unsigned char *font);

// Buffer for printf
static char console_printf_buffer[512];

// VGA register access
static inline void vga_write_reg(uint16_t iport, uint8_t reg, uint8_t val){
	io_outb(iport, reg);
	io_outb(iport + 1, val);
}

static inline uint8_t vga_read_reg(uint16_t iport, uint8_t reg){
	io_outb(iport, reg); 
	return io_inb(iport + 1);
}

// Updates the position of the cursor
static void vga_move_cursor();

// Sequencer IO Ports
#define VGA_SEQ_INDEX_PORT			0x3C4
#define VGA_SEQ_DATA_PORT			0x3C5

// Graphics Controller IO Ports (VRAM read/write)
#define VGA_GC_INDEX_PORT			0x3CE
#define VGA_GC_DATA_PORT			0x3CF

// Attribute Controller IO Ports
#define	VGA_AC_INDEX_PORT			0x3C0
#define	VGA_AC_WRITE_PORT			0x3C0
#define	VGA_AC_READ_PORT			0x3C1

// CRT controller IO Ports (timing generator)
#define VGA_CRTC_INDEX_PORT			0x3D4
#define VGA_CRTC_DATA_PORT			0x3D5

// VGA DAC control IO ports
#define VGA_DAC_READ_INDEX_PORT		0x3C7
#define VGA_DAC_WRITE_INDEX_PORT	0x3C8
#define VGA_DAC_DATA_PORT			0x3C9

// Sequencer registers
#define VGA_SEQ_MAP_MASK_REG		0x02
#define VGA_SEQ_CHARSET_REG			0x03
#define VGA_SEQ_MEMORY_MODE_REG		0x04

// Graphic controller registers
#define VGA_GC_READ_MAP_SELECT_REG	0x04
#define VGA_GC_GRAPHICS_MODE_REG	0x05
#define VGA_GC_MISC_REG				0x06

// Miscellaneous register write
#define	VGA_MISC_WRITE				0x3C2

/**
 * Clears the console.
 */
void platform_console_clear(void) {
	current_x = current_y = 0;

	// clear the VGA memory (0xB8000)
	uint16_t *mem = (uint16_t *) 0xB8000;
	memset(mem, 0x00, CONSOLE_WIDTH * CONSOLE_HEIGHT * 2);

	// load custom font
	vga_load_font((unsigned char *) &vga_font_bold);
}

/**
 * Prints a character to the current console. This can print to the display,
 * a debugger, a serial interface, and more. Special characters like "\n" must
 * be handled specifically.
 */
void platform_console_putc(char c) {
	// Check if special character
	if(c == '\n') {
		current_y++;
		current_x = 0;

		// check if the console needs to be scrolled up
		if(current_y == CONSOLE_HEIGHT) {
			uint16_t *src = (uint16_t *) (0xB8000 + (CONSOLE_WIDTH * 2));
			uint16_t *dst = (uint16_t *) 0xB8000;
			memcpy(dst, src, CONSOLE_WIDTH * (CONSOLE_HEIGHT - 1) * 2);

			dst = (uint16_t *) (0xB8000 + CONSOLE_WIDTH * (CONSOLE_HEIGHT - 2));
			memset(dst, 0x00, CONSOLE_WIDTH * 2);
		}
	} else { // regular character
		uint16_t *mem = (uint16_t *) (0xB8000 + (current_y * CONSOLE_WIDTH * 2) + (current_x * 2));
		*mem = (attrib << 8) | c;

		current_x++;

		// wrap X?
		if(current_x == CONSOLE_WIDTH) {
			current_x = 0;
			current_y++;

			// check if the console needs to be scrolled up
			if(current_y == CONSOLE_HEIGHT) {
				uint16_t *src = (uint16_t *) (0xB8000 + (CONSOLE_WIDTH * 2));
				uint16_t *dst = (uint16_t *) 0xB8000;
				memcpy(dst, src, CONSOLE_WIDTH * (CONSOLE_HEIGHT - 1) * 2);

				dst = (uint16_t *) (0xB8000 + CONSOLE_WIDTH * (CONSOLE_HEIGHT - 2));
				memset(dst, 0x00, CONSOLE_WIDTH * 2);
			}
		}
	}

	// update cursor
	vga_move_cursor();
}

/**
 * Prints a formatted string to the current console. This works like printf on
 * most systems.
 */
extern int platform_console_printf(const char* format, va_list ap) {
	char *str = (char *) console_printf_buffer;
	int n = vsprintf(str, format, ap);

	// rpint the char
	while(*str) {
		platform_console_putc(*str++);
	}

	return n;
}

/**
 * Changes the type of message to be printed to the console. Consoles needn't
 * do anything with this, but a possible use may be printing coloured text.
 */
void platform_console_set_type(platform_console_type_t type) {
	switch(type) {
		// white on red
		case kPlatformConsoleCritical:
			attrib = 0xCF;
			break;

		// red on black
		case kPlatformConsoleCriticalError:
			attrib = 0x0C;
			break;

		// orange/brown on black
		case kPlatformConsoleError:
			attrib = 0x06;
			break;

		// yellow on black
		case kPlatformConsoleWarning:
			attrib = 0x0E;
			break;

		// green on black
		case kPlatformConsoleSuccess:
			attrib = 0x0A;
			break;

		// white on black
		case kPlatformConsoleNormal:
			attrib = 0x07;
			break;

		// blue on black
		case kPlatformConsoleDebug:
			attrib = 0x0B;
			break;

		// white on blue
		case kPlatformConsoleEmphasis:
			attrib = 0xBF;
			break;

		default:
			break;
	}
}

/*
 * Loads a new font into the VGA hardware.
 */
static void vga_load_font(unsigned char *font) {
	unsigned char *p = (unsigned char *) 0xB8000;
	
	// Enable writing to plane 2
	vga_write_reg(VGA_SEQ_INDEX_PORT, VGA_SEQ_MAP_MASK_REG, 0x04);
	
	// Ensure first font is selected
	vga_write_reg(VGA_SEQ_INDEX_PORT, VGA_SEQ_CHARSET_REG, 0x00);
	
	// Enable sequential memory access
	uint8_t mem_mode = vga_read_reg(VGA_SEQ_INDEX_PORT, VGA_SEQ_MEMORY_MODE_REG);
	vga_write_reg(VGA_SEQ_INDEX_PORT, VGA_SEQ_MEMORY_MODE_REG, 0x06);
	
	vga_write_reg(VGA_GC_INDEX_PORT, VGA_GC_READ_MAP_SELECT_REG, 0x02);
	
	// Set up the graphics mode
	uint8_t graphics_mode = vga_read_reg(VGA_GC_INDEX_PORT, VGA_GC_GRAPHICS_MODE_REG);
	vga_write_reg(VGA_GC_INDEX_PORT, VGA_GC_GRAPHICS_MODE_REG, 0x00);
	

	vga_write_reg(VGA_GC_INDEX_PORT, VGA_GC_MISC_REG, 0x0C);
	
	// Write each character
	for(unsigned int i = 0; i < 256; i++){
		for(unsigned int j = 0; j < 16; j++){
			*p++= *font++;
		}

		// Skip 16 bytes between glyphs
		p += 16;
	}
	
	// Enable write to planes 0 and 1
	vga_write_reg(VGA_SEQ_INDEX_PORT, VGA_SEQ_MAP_MASK_REG, 0x03);
	vga_write_reg(VGA_SEQ_INDEX_PORT, VGA_SEQ_MEMORY_MODE_REG, mem_mode);
	
	vga_write_reg(VGA_GC_INDEX_PORT, VGA_GC_READ_MAP_SELECT_REG, 0x00);
	vga_write_reg(VGA_GC_INDEX_PORT, VGA_GC_GRAPHICS_MODE_REG, graphics_mode);

	// Restore VGA text mode
	vga_write_reg(VGA_GC_INDEX_PORT, VGA_GC_MISC_REG, 0x0C);	
}

/*
 * Set cursor to current X and Y coordinate
 */
static void vga_move_cursor() {
	uint16_t cursorLocation = (current_y * CONSOLE_WIDTH) + current_x;

	// Set high cursor byte
	io_outb(VGA_CRTC_INDEX_PORT, 14);
	io_outb(VGA_CRTC_DATA_PORT, cursorLocation >> 8);

	// Set low cursor byte
	io_outb(VGA_CRTC_INDEX_PORT, 15);
	io_outb(VGA_CRTC_DATA_PORT, cursorLocation);
}