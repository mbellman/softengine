#include <Graphics/ColorBuffer.h>
#include <algorithm>
#include <Helpers.h>
#include <Constants.h>

/**
 * ColorBuffer
 * -----------
 */
ColorBuffer::ColorBuffer(int width, int height): width(width), height(height) {
	buffer = new Color[width * height];
	bufferSize = width * height;
}

ColorBuffer::~ColorBuffer() {
	delete[] buffer;
}

ColorBuffer* ColorBuffer::createDownsizedBuffer() {
	if (width <= 2 || height <= 2) {
		return this;
	}

	ColorBuffer* colorBuffer = new ColorBuffer(width >> 1, height >> 1);

	for (int y = 0; y < height - 1; y +=2) {
		for (int x = 0; x < width - 1; x += 2) {
			const Color& tL = read(x, y);
			const Color& tR = read(x + 1, y);
			const Color& bL = read(x, y + 1);
			const Color& bR = read(x + 1, y + 1);

			int averageR = (int)((tL.R + tR.R + bL.R + bR.R) / 4.0f);
			int averageG = (int)((tL.G + tR.G + bL.G + bR.G) / 4.0f);
			int averageB = (int)((tL.B + tR.B + bL.B + bR.B) / 4.0f);

			colorBuffer->write(x >> 1, y >> 1, averageR, averageG, averageB);
		}
	}

	return colorBuffer;
}

inline int ColorBuffer::getIndex(int x, int y) const {
	if (x >= width || y >= height) {
		return 0;
	}

	return width * y + x;
}

const Color& ColorBuffer::read(int index) const {
	return (index < 0 || index >= bufferSize) ? COLOR_BLACK : buffer[index];
}

const Color& ColorBuffer::read(int x, int y) const {
	return read(getIndex(x, y));
}

void ColorBuffer::write(int x, int y, int R, int G, int B) {
	Color color = { R, G, B };

	buffer[getIndex(x, y)] = color;
}
