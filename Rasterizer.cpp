#include <math.h>
#include <stdio.h>
#include <algorithm>
#include <Helpers.h>
#include <Rasterizer.h>

Rasterizer::Rasterizer(SDL_Renderer* renderer, int width, int height) {
	this->width = width;
	this->height = height;

	color = 0;
	screenTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, width, height);
	pixelBuffer = new Uint32[width * height];
	depthBuffer = new int[width * height];

	setColor(255, 255, 255);
}

Rasterizer::~Rasterizer() {
	SDL_DestroyTexture(screenTexture);

	delete[] pixelBuffer;
	delete[] depthBuffer;
}

void Rasterizer::clear() {
	int bufferLength = width * height;

	std::fill(pixelBuffer, pixelBuffer + bufferLength, 0);
	std::fill(depthBuffer, depthBuffer + bufferLength, 0);
}

void Rasterizer::flatBottomTriangle(const Vertex2d& top, const Vertex2d& bottomLeft, const Vertex2d& bottomRight) {
	int triangleHeight = bottomLeft.coordinate.y - top.coordinate.y;
	float leftSlope = (float)triangleHeight / (bottomLeft.coordinate.x - top.coordinate.x);
	float rightSlope = (float)triangleHeight / (bottomRight.coordinate.x - top.coordinate.x);

	for (int i = 0; i < triangleHeight; i++) {
		int y = top.coordinate.y + i;

		if (y < 0) {
			continue;
		}
		else if (y > height) {
			break;
		}

		float progress = (float)i / triangleHeight;
		int start = top.coordinate.x + (int)i / leftSlope;
		int end = top.coordinate.x + (int)i / rightSlope;
		Color startColor = lerp(top.color, bottomLeft.color, progress);
		Color endColor = lerp(top.color, bottomRight.color, progress);

		triangleScanLine(start, y, end - start, startColor, endColor);
	}
}

void Rasterizer::flatTopTriangle(const Vertex2d& topLeft, const Vertex2d& topRight, const Vertex2d& bottom) {
	int triangleHeight = bottom.coordinate.y - topLeft.coordinate.y;
	float leftSlope = (float)triangleHeight / (bottom.coordinate.x - topLeft.coordinate.x);
	float rightSlope = (float)triangleHeight / (bottom.coordinate.x - topRight.coordinate.x);

	for (int i = 0; i < triangleHeight; i++) {
		int y = topLeft.coordinate.y + i;

		if (y < 0) {
			continue;
		}
		else if (y > height) {
			break;
		}

		float progress = (float)i / triangleHeight;
		int start = topLeft.coordinate.x + (int)i / leftSlope;
		int end = topRight.coordinate.x + (int)i / rightSlope;
		Color startColor = lerp(topLeft.color, bottom.color, progress);
		Color endColor = lerp(topRight.color, bottom.color, progress);

		triangleScanLine(start, y, end - start, startColor, endColor);
	}
}

void Rasterizer::line(int x1, int y1, int x2, int y2) {
	int deltaX = x2 - x1;
	int deltaY = y2 - y1;
	int totalPixels = abs(deltaX) + abs(deltaY);

	for (int i = 0; i < totalPixels; i++) {
		float progress = (float)i / totalPixels;
		int x = x1 + (int)(deltaX * progress);
		int y = y1 + (int)(deltaY * progress);

		if (x < 0 || x >= width || y < 0 || y >= height) {
			continue;
		}

		bool isGoingOffScreen = (
			deltaX < 0 && x < 0 ||
			deltaX > 0 && x > width ||
			deltaY < 0 && y < 0 ||
			deltaY > 0 && y > height
		);

		if (isGoingOffScreen) {
			break;
		}

		setPixel(x, y);
	}
}

void Rasterizer::render(SDL_Renderer* renderer) {
	SDL_UpdateTexture(screenTexture, NULL, pixelBuffer, width * sizeof(Uint32));
	SDL_RenderCopy(renderer, screenTexture, NULL, NULL);
	SDL_RenderPresent(renderer);

	clear();
}

void Rasterizer::setColor(int R, int G, int B) {
	color = (255 << 24) | (R << 16) | (G << 8) | B;
}

void Rasterizer::setColor(Color* color) {
	setColor(color->R, color->G, color->B);
}

void Rasterizer::setPixel(int x, int y, int depth) {
	int index = y * width + x;

	pixelBuffer[index] = color;
	depthBuffer[index] = depth;
}

void Rasterizer::triangle(int x1, int y1, int x2, int y2, int x3, int y3) {
	line(x1, y1, x2, y2);
	line(x2, y2, x3, y3);
	line(x3, y3, x1, y1);
}

/**
 * Rasterize a filled triangle with per-vertex coloration.
 */
void Rasterizer::triangle(Triangle& triangle) {
	Vertex2d* top = &triangle.vertices[0];
	Vertex2d* middle = &triangle.vertices[1];
	Vertex2d* bottom = &triangle.vertices[2];

	if (top->coordinate.y > middle->coordinate.y) {
		std::swap(top, middle);
	}

	if (middle->coordinate.y > bottom->coordinate.y) {
		std::swap(middle, bottom);
	}

	if (top->coordinate.y > middle->coordinate.y) {
		std::swap(top, middle);
	}

	if (top->coordinate.y == middle->coordinate.y) {
		if (top->coordinate.x > middle->coordinate.x) {
			std::swap(top, middle);
		}

		flatTopTriangle(*top, *middle, *bottom);
	} else if (bottom->coordinate.y == middle->coordinate.y) {
		if (bottom->coordinate.x < middle->coordinate.x) {
			std::swap(bottom, middle);
		}

		flatBottomTriangle(*top, *middle, *bottom);
	} else {
		float ttbSlope = (float)(bottom->coordinate.y - top->coordinate.y) / (bottom->coordinate.x - top->coordinate.x);
		float middleYRatio = (float)(middle->coordinate.y - top->coordinate.y) / (bottom->coordinate.y - top->coordinate.y);

		Vertex2d middleOpposite;

		middleOpposite.color = lerp(top->color, bottom->color, middleYRatio);
		middleOpposite.coordinate = { top->coordinate.x + (int)((middle->coordinate.y - top->coordinate.y) / ttbSlope), middle->coordinate.y };

		Vertex2d* middleLeft = middle;
		Vertex2d* middleRight = &middleOpposite;

		if (middleLeft->coordinate.x > middleRight->coordinate.x) {
			std::swap(middleLeft, middleRight);
		}

		flatBottomTriangle(*top, *middleLeft, *middleRight);
		flatTopTriangle(*middleLeft, *middleRight, *bottom);
	}
}

void Rasterizer::triangleScanLine(int x1, int y1, int lineLength, const Color& leftColor, const Color& rightColor) {
	for (int x = x1; x <= x1 + lineLength; x++) {
		if (x < 0 || depthBuffer[y1 * this->width + x] > 0) {
			continue;
		}
		else if (x > width) {
			break;
		}

		float progress = (float)(x - x1) / lineLength;
		int R = lerp(leftColor.R, rightColor.R, progress);
		int G = lerp(leftColor.G, rightColor.G, progress);
		int B = lerp(leftColor.B, rightColor.B, progress);

		setColor(R, G, B);
		setPixel(x, y1);
	}
}