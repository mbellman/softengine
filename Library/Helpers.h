#pragma once

/**
 * Preprocessors for speeding up code when built in Visual Studio.
 */
#define FAST_CLAMP(v, l, h) v < l ? l : v > h ? h : v
#define FAST_MAX(v1, v2) v1 > v2 ? v1 : v2
#define FAST_MIN(v1, v2) v1 < v2 ? v1 : v2

#include <SDL.h>
#include <Types.h>

inline int lerp(int v1, int v2, float ratio) {
	return v1 + (int)(v2 - v1) * ratio;
}

inline Color lerp(const Color& c1, const Color& c2, float ratio) {
	return {
		lerp(c1.R, c2.R, ratio),
		lerp(c1.G, c2.G, ratio),
		lerp(c1.B, c2.B, ratio)
	};
}
