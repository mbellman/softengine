#pragma once

#include <SDL.h>

/**
 * Flags
 * -----
 */
enum Flags : Uint32 {
	DEBUG_DRAWTIME = 1 << 0,
	SHOW_WIREFRAME = 1 << 1,
	FLAT_SHADING = 1 << 2,
	PIXEL_FILTER = 1 << 3,
	DISABLE_MULTITHREADING = 1 << 4,
	DISABLE_MIPMAPPING = 1 << 5
};