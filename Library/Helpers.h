#pragma once

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

inline int clamp(int v, int low, int high) {
	return v < low ? low : v > high ? high : v;
}

inline float clamp(float v, float low, float high) {
	return v < low ? low : v > high ? high : v;
}
