#pragma once

#include <random>
#include <math.h>

/**
 * Preprocessors for speeding up code when built in Visual Studio.
 */
#define FAST_CLAMP(v, l, h) (v < l ? l : v > h ? h : v)
#define FAST_MAX(v1, v2) (v1 > v2 ? v1 : v2)
#define FAST_MIN(v1, v2) (v1 < v2 ? v1 : v2)
#define ARGB(r, g, b) (255 << 24) | (r << 16) | (g << 8) | b

namespace Lerp {
	inline int lerp(int v1, int v2, float ratio) {
		return v1 + (int)(v2 - v1) * ratio;
	}

	inline float lerp(float v1, float v2, float ratio) {
		return v1 + (v2 - v1) * ratio;
	}
};

namespace RNG {
	float random(float low, float high);
};
