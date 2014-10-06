#include "x86.h"
#include "multiboot.h"

// physical location of multiboot struct
void *x86_platform_multiboot_struct_addr;

// memory structure
static platform_bootargs_t bootargs;

/**
 * Parses the boot argument structure. This assumes that the bootup/init handler
 * fetched appropriate information.
 */
extern void platform_bootarg_parse(void) {
	multiboot_info_t *info = (multiboot_info_t *) x86_platform_multiboot_struct_addr;

	// available mem
	bootargs.total_mem = info->mem_upper;
	strncpy((char *) &bootargs.boot_params, (char *) info->cmdline, 512);

	// physical load address (fixed for x86 with multiboot)
	bootargs.load_address_phys = 0x00100000;

	// did we get VBE info?
	if(info->flags & MULTIBOOT_INFO_VIDEO_INFO) {
		multiboot_vbe_t *modeInfo = (multiboot_vbe_t *) info->vbe_mode_info;
		multiboot_vbe_control_t *vbeInfo = (multiboot_vbe_control_t *) info->vbe_control_info;

		bootargs.framebuffer.video_mem = vbeInfo->video_mem * 64;

		// get some basic information, like display mode size
		bootargs.framebuffer.isVideo = (modeInfo->attributes & 0x10);

		bootargs.framebuffer.width = modeInfo->screen_width;
		bootargs.framebuffer.height = modeInfo->screen_height;

		bootargs.framebuffer.stride = modeInfo->pitch;

		switch(modeInfo->bpp) {
			case 16:
				bootargs.framebuffer.pixel_format = kFramebufferRGB565;
				break;

			case 32:
				bootargs.framebuffer.pixel_format = kFramebufferRGBA8888;
				break;

			default:
				KERROR("Unsupported bpp: %u\n", modeInfo->bpp);
		}

		// get linear address
		bootargs.framebuffer.base = (uintptr_t) modeInfo->physbase;

		//KDEBUG("Res %ux%u\n", bootargs.framebuffer.width, bootargs.framebuffer.height);
		//KDEBUG("Base at 0x%X\n", (unsigned int) bootargs.framebuffer.base);
	}
}

/**
 * Returns a pointer to the boot argument structure. It contains information on
 * various platform specifities, like what peripherals are available, and other
 * such things.
 */
const platform_bootargs_t *platform_bootarg_get(void) {
	return &bootargs;
}