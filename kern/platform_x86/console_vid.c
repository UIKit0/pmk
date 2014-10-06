#include "x86.h"
#include "rsrc/ter-i16n.h"

#include "vm/vm.h"

// Current X/Y position
static int current_x, current_y;

// Video memory base
static uintptr_t lfb_base;

// size of console
static int CONSOLE_WIDTH, CONSOLE_HEIGHT;
static unsigned int stride;

// Buffer for printf
static char console_printf_buffer[512];

// colours for text
static unsigned int fg_colour, bg_colour;

// whether console is initialised or not
static bool is_initialised = false;

/**
 * Scrolls the display up one row.
 */
static void scroll_up(void) {	
	return;

	uint8_t *video_memory = (uint8_t *) 0xB8000;

	// Move everything upwards
	memmove(video_memory, video_memory + (CONSOLE_WIDTH * 2), (CONSOLE_HEIGHT - 1) * CONSOLE_WIDTH*2);

	// Clear bottom row
	memclr(video_memory + ((CONSOLE_HEIGHT - 1) * (2 * CONSOLE_WIDTH)), CONSOLE_WIDTH * 2);

	current_y--;
}

/**
 * Maps the video console's memory.
 */
void platform_console_vid_map(void) {
	const platform_bootargs_t *args = platform_bootarg_get();

	CONSOLE_WIDTH = args->framebuffer.width / 8;
	CONSOLE_HEIGHT = args->framebuffer.height / 16;
	stride = args->framebuffer.stride;

	// get base of framebuffer
	lfb_base = args->framebuffer.base;

	// now, map the LFB into the platform region
	for (int i = 0; i < 0x400000; i += 0x1000) {
		platform_pm_map(platform_pm_get_kernel_table(), i + 0xF0000000, lfb_base + i, VM_FLAGS_KERNEL);
	}

	// we map framebuffer to 0xF0000000
	lfb_base = 0xF0000000;

	is_initialised = true;
}

/**
 * Clears the console.
 */
void platform_console_vid_clear(void) {
	if(unlikely(!is_initialised)) return;

	// clear framebuffer to black
	uint32_t *mem = (uint32_t *) lfb_base;
	memset(mem, 0x00, CONSOLE_WIDTH * CONSOLE_HEIGHT * 8 * 16 * 4);

	current_x = current_y = 0;

	// default colour
	fg_colour = 0xFFFFFFFF;
	bg_colour = 0x00000000;
}

/**
 * Prints a character to the current console. This can print to the display,
 * a debugger, a serial interface, and more. Special characters like "\n" must
 * be handled specifically.
 */
void platform_console_vid_putc(char c) {
//	if(unlikely(!is_initialised)) return;

	// Check if special character
	if(c == '\n') {
		current_y++;
		current_x = 0;

		// check if the console needs to be scrolled up
		if(current_y == CONSOLE_HEIGHT) {
			scroll_up();
		}
	} else { // regular character
		static const unsigned int table[8] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};
		uint8_t *read_ptr = (uint8_t *) ter_i16n_raw + (c * 16);

		// char size 8x16
		for(int y = 0; y < 16; y++) {
			uint8_t row = read_ptr[y];

			// calculate base address for row
			uint32_t *mem = (uint32_t *) lfb_base + (y * (stride / 4)) + (current_y * 16 * stride) + (current_x * 8);

			// process each column
			for(int x = 0; x < 8; x++) {
				if(row & table[x]) {
					mem[x] = fg_colour;
				} else {
					mem[x] = bg_colour;
				}
			}
		}

		current_x++;
		// wrap X?

		if(current_x == CONSOLE_WIDTH) {
			current_x = 0;
			current_y++;

			// check if the console needs to be scrolled up
			if(current_y == CONSOLE_HEIGHT) {
				scroll_up();
			}
		}
	}
}

/**
 * Prints a formatted string to the current console. This works like printf on
 * most systems.
 */
extern int platform_console_vid_printf(const char* format, va_list ap) {
	char *str = (char *) console_printf_buffer;
	int n = vsprintf(str, format, ap);

	// rpint the char
	while(*str) {
		platform_console_vid_putc(*str++);
	}

	return n;
}

/**
 * Changes the type of message to be printed to the console. Consoles needn't
 * do anything with this, but a possible use may be printing coloured text.
 */
void platform_console_vid_set_type(platform_console_type_t type) {
	switch(type) {
		// white on red
		case kPlatformConsoleCritical:
			fg_colour = 0x00FFFFFF;
			bg_colour = 0x00FF0000;
			break;

		// red on black
		case kPlatformConsoleCriticalError:
			fg_colour = 0x00FF0000;
			bg_colour = 0x00000000;
			break;

		// orange/brown on black
		case kPlatformConsoleError:
			fg_colour = 0x00FF8800;
			bg_colour = 0x00000000;
			break;

		// yellow on black
		case kPlatformConsoleWarning:
			fg_colour = 0x00FFFF00;
			bg_colour = 0x00000000;
			break;

		// green on black
		case kPlatformConsoleSuccess:
			fg_colour = 0x0000FF00;
			bg_colour = 0x00000000;
			break;

		// white on black
		case kPlatformConsoleNormal:
			fg_colour = 0xFFFFFFFF;
			bg_colour = 0x00000000;
			break;

		// blue on black
		case kPlatformConsoleDebug:
			fg_colour = 0x007777FF;
			bg_colour = 0x00000000;
			break;

		// white on blue
		case kPlatformConsoleEmphasis:
			fg_colour = 0xFFFFFF00;
			bg_colour = 0x0000FF00;
			break;

		default:
			break;
	}
}