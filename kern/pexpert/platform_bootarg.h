#ifndef PLATFORM_BOOTARG_H
#define PLATFORM_BOOTARG_H

/**
 * This structure contains information on boot arguments.
 */
struct platform_bootargs {
	unsigned int total_mem; // in kilobytes
	uintptr_t load_address_phys;

	char boot_params[512];

	struct {
		bool isVideo;

		unsigned int width, height;
		unsigned int stride;

		enum {
			kFramebufferRGBA8888,
			kFramebufferRGB565,
		} pixel_format;

		uintptr_t base;

		unsigned int video_mem; // Kbytes
	} framebuffer;
};
typedef struct platform_bootargs platform_bootargs_t;

/**
 * Parses the boot argument structure. This assumes that the bootup/init handler
 * fetched appropriate information.
 */
extern void platform_bootarg_parse(void);

/**
 * Returns a pointer to the boot argument structure. It contains information on
 * various platform specifities, like what peripherals are available, and other
 * such things.
 */
extern const platform_bootargs_t *platform_bootarg_get(void);

#endif