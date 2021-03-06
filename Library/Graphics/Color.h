#pragma once

#include <System/Math.h>

/**
 * Color
 * -----
 */
struct Color {
	int R = 255;
	int G = 255;
	int B = 255;

	static Color lerp(const Color& c1, const Color& c2, float r);

	void clamp();
	Vec3 ratios() const;
	Color operator +(int attenuation) const;
	Color operator +(const Color& color) const;
	Color operator +=(const Color& color);
	Color operator -(int attenuation) const;
	Color operator -(const Color& color) const;
	Color operator -=(const Color& color);
	Color operator *(float multiplier) const;
	Color operator *=(float multiplier);

private:
	static int normalize(int component);
};

/**
 * Colorable
 * ---------
 */
struct Colorable {
	Color color;
};
