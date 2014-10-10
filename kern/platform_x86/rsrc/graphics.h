#ifndef X86_PLATFORM_CONSOLE_GRAPHICS_H
#define X86_PLATFORM_CONSOLE_GRAPHICS_H

// each graphic is encapsulated in this type
typedef struct {
	unsigned int width, height;
	unsigned int frames, fps;
	unsigned int yOffset;

	const uint8_t *image;
	const uint8_t *clut;
} x86_platform_graphic_t;

// import the default CLUT
#include "clut.h"

// define boot logo graphic
#include "logo.h"

static const x86_platform_graphic_t gBootLogo = {
	.width = kBootLogoWidth,
	.height = kBootLogoHeight,

	.image = graphic_bootLogoImage,
	.clut = graphic_default_clut
};

// panic cross
#include "panic.h"

static const x86_platform_graphic_t gPanicCross = {
	.width = kPanicWidth,
	.height = kPanicHeight,

	.image = graphic_panicCross,
	.clut = graphic_default_clut
};

// loading gear (animated)
/*#include "gear.h"

static const x86_platform_graphic_t gLoadingGear = {
	.width = kGearWidth,
	.height = kGearHeight,

	.frames = kGearFrames,
	.fps = kGearFPS,

	.yOffset = 4,

	.image = graphic_loadingGear,
	.clut = graphic_default_clut
};*/

#endif