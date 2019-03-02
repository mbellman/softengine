#pragma once

#include <SDL.h>

/**
 * Flags
 * -----
 */
enum Flags : Uint32 {
	DEBUG_STATS = 1 << 0,
	SHOW_WIREFRAME = 1 << 1,
	PIXEL_FILTER = 1 << 2,
	DISABLE_MULTITHREADING = 1 << 3,
	FPS_30 = 1 << 4
};
