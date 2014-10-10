#include "x86.h"

#include "rsrc/ter-i16n.h"
#include "rsrc/ter-i16b.h"
#include "rsrc/graphics.h"

#include "vm/vm.h"

// Current X/Y position
static unsigned int current_x, current_y;

// Video memory base
static uintptr_t lfb_base;
static uintptr_t lfb_txt_base;

// size of console
static unsigned int CONSOLE_WIDTH, CONSOLE_HEIGHT;
static unsigned int stride;

// Buffer for printf
static char console_printf_buffer[512];

// colours for text
static unsigned int fg_colour, bg_colour;

// whether console is initialised or not
static bool is_initialised = false;

// which frame from the progress indicator is being displayed
static unsigned int progress_frame;

/**
 * Scrolls the display up one row.
 */
static void scroll_up(void) {	
	uint32_t *dst = (uint32_t *) lfb_txt_base + (0 * 16 * stride);
	uint32_t *src = (uint32_t *) lfb_txt_base + (1 * 16 * stride);
	
	// copy row 2 to 1 and so forth
	memmove(dst, src, CONSOLE_WIDTH * (16 * (CONSOLE_HEIGHT - 1)));

	// clear last row
	dst = (uint32_t *) lfb_txt_base + ((CONSOLE_HEIGHT - 1) * 16 * stride);
	memset(dst, 0, CONSOLE_WIDTH * 4 * 16);

	current_y--;
}

/**
 * Draws the current loading gear frame.
 */
void draw_progress_bar(void) {
	return;

/*	const platform_bootargs_t *args = platform_bootarg_get();

	unsigned int videoAreaHeight = (args->framebuffer.height / 4) * 3;
	unsigned int logoStartY = (videoAreaHeight / 2) + (gBootLogo.height / 2) + gLoadingGear.yOffset;
	unsigned int logoStartX = (args->framebuffer.width / 2) - (gLoadingGear.width / 2);

	for(int y = 0; y < gLoadingGear.height; y++) {
		uint32_t *ptr = (uint32_t *) lfb_base + (y * (stride / 4)) + (logoStartX) + (logoStartY * (stride / 4));

		for(int x = 0; x < gLoadingGear.width; x++) {
			uint8_t colour = gLoadingGear.image[(gear_frame * gLoadingGear.width * gLoadingGear.height) + (gLoadingGear.width * y) + x];

			uint8_t r = gLoadingGear.clut[(colour * 3)];
			uint8_t g = gLoadingGear.clut[(colour * 3) + 1];
			uint8_t b = gLoadingGear.clut[(colour * 3) + 2];

			ptr[x] = (r << 16) | (g << 8) | (b << 0);
		}
	}*/
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
	for (int i = 0; i < 0x800000; i += 0x1000) {
		platform_pm_map(platform_pm_get_kernel_table(), i + 0xF0000000, lfb_base + i, VM_FLAGS_KERNEL);
	}

	int heightSub = (CONSOLE_HEIGHT / 4) * 3;

	// we map framebuffer to 0xF0000000
	lfb_base = 0xF0000000;
	lfb_txt_base = lfb_base + (16 * stride * heightSub);

	CONSOLE_HEIGHT -= heightSub;

	is_initialised = true;
}

/**
 * Clears the console.
 */
void platform_console_vid_clear(void) {
	if(unlikely(!is_initialised)) return;
	const platform_bootargs_t *args = platform_bootarg_get();

	// clear text area
	uint32_t *mem = (uint32_t *) lfb_txt_base;
	memset(mem, 0x00, stride * CONSOLE_HEIGHT * 16);

	// clear video area at top
	mem = (uint32_t *) lfb_base;
	memset(mem, 0xBF, stride * (args->framebuffer.height / 4) * 3);

	// draw a line right at the end of the video area
	mem = (uint32_t *) (lfb_txt_base - stride);
	memset(mem, 0xFF, stride);

	// we wish to draw the boot logo (128x128) centered in the grey area
	unsigned int videoAreaHeight = (args->framebuffer.height / 4) * 3;
	unsigned int logoStartY = (videoAreaHeight / 2) - (gBootLogo.height / 2);
	unsigned int logoStartX = (args->framebuffer.width / 2) - (gBootLogo.width / 2);

	for(int y = 0; y < gBootLogo.height; y++) {
		uint32_t *ptr = (uint32_t *) lfb_base + (y * (stride / 4)) + (logoStartX) + (logoStartY * (stride / 4));

		for(int x = 0; x < gBootLogo.width; x++) {
			uint8_t colour = gBootLogo.image[(gBootLogo.width * y) + x];

			uint8_t r = gBootLogo.clut[(colour * 3)];
			uint8_t g = gBootLogo.clut[(colour * 3) + 1];
			uint8_t b = gBootLogo.clut[(colour * 3) + 2];

			ptr[x] = (r << 16) | (g << 8) | (b << 0);
		}
	}

	current_x = current_y = 0;

	// default colour
	fg_colour = 0xFFFFFFFF;
	bg_colour = 0x00000000;

	// draw frame 0 of the gears
	progress_frame = 0;
	draw_progress_bar();
}

/**
 * Prints a character to the current console. This can print to the display,
 * a debugger, a serial interface, and more. Special characters like "\n" must
 * be handled specifically.
 */
void platform_console_vid_putc(char c) {
	if(unlikely(!is_initialised)) return;

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
		uint8_t *read_ptr = (uint8_t *) ter_i16b_raw + (c * 16);

		// char size 8x16
		for(int y = 0; y < 16; y++) {
			uint8_t row = read_ptr[y];

			// calculate base address for row
			uint32_t *mem = (uint32_t *) lfb_txt_base + (y * (stride / 4)) + (current_y * 4 * stride) + (current_x * 8);

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