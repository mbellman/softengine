#pragma once

/**
 * Flags
 * -----
 */
enum Flags {
	DEBUG_STATS = 1 << 0,
	DEBUG_COMMAND_LINE = 1 << 1,
	SHOW_WIREFRAME = 1 << 2,
	PIXEL_FILTER = 1 << 3,
	DISABLE_MULTITHREADING = 1 << 4,
	DISABLE_WINDOW_RESIZE = 1 << 5,
	FPS_30 = 1 << 6
};
