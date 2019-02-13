#pragma once

#include <SDL.h>
#include <Graphics/Color.h>

/**
 * ColorBuffer
 * -----------
 *
 * A buffer of Color objects representing texture data.
 */
class ColorBuffer {
public:
	const int width;
	const int height;

	ColorBuffer(int width, int height);
	~ColorBuffer();

	ColorBuffer* createDownsizedBuffer();
	const Color& read(int index) const;
	const Color& read(int x, int y) const;
	void write(int x, int y, int R, int G, int B);

private:
	inline int getIndex(int x, int y) const;

	Color BLACK = { 0, 0, 0 };
	Color* buffer = NULL;
	int bufferSize;
};
