#include <Graphics/Rasterizer.h>
#include <math.h>
#include <stdio.h>
#include <algorithm>
#include <limits.h>
#include <Helpers.h>
#include <Graphics/TextureBuffer.h>
#include <System/Geometry.h>
#include <System/Objects.h>

using namespace std;

/**
 * Rasterizer
 * ----------
 */
Rasterizer::Rasterizer(SDL_Renderer* renderer, int width, int height, bool shouldUsePerVertexColoration) {
	this->width = width;
	this->height = height;
	this->shouldUsePerVertexColoration = shouldUsePerVertexColoration;

	screenTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, width, height);
	pixelBuffer = new Uint32[width * height];
	depthBuffer = new int[width * height];

	setColor(255, 255, 255);
	clear();
}

Rasterizer::~Rasterizer() {
	SDL_DestroyTexture(screenTexture);

	delete[] pixelBuffer;
	delete[] depthBuffer;
}

void Rasterizer::clear() {
	int bufferLength = width * height;

	fill(pixelBuffer, pixelBuffer + bufferLength, backgroundColor);
	fill(depthBuffer, depthBuffer + bufferLength, INT_MAX);
}

void Rasterizer::flatTriangle(const Vertex2d& corner, const Vertex2d& left, const Vertex2d& right, const TextureBuffer* texture) {
	int isHorizontallyOffscreen = (
		(corner.coordinate.x >= width && left.coordinate.x >= width) ||
		(corner.coordinate.x < 0 && right.coordinate.x < 0)
	);

	if (isHorizontallyOffscreen) {
		return;
	}

	int triangleHeight = abs(left.coordinate.y - corner.coordinate.y);
	int topY = FAST_MIN(corner.coordinate.y, left.coordinate.y);
	int start = FAST_MAX(topY, 0);
	int end = FAST_MIN(topY + triangleHeight, height);
	bool hasFlatTop = corner.coordinate.y > left.coordinate.y;

	Color startColor, endColor;
	Vec2 startUV, endUV;
	float startW = 0.0f, endW = 0.0f;

	for (int y = start; y < end; y++) {
		int step = hasFlatTop ? (triangleHeight - (y - topY)) : (y - topY);
		float progress = (float)step / triangleHeight;
		int startX = Lerp::lerp(corner.coordinate.x, left.coordinate.x, progress);
		int endX = Lerp::lerp(corner.coordinate.x, right.coordinate.x, progress);
		int startDepth = Lerp::lerp(corner.depth, left.depth, progress);
		int endDepth = Lerp::lerp(corner.depth, right.depth, progress);
		int lineLength = endX - startX;

		// Lerp color components individually instead of Color::lerp
		// for a small performance gain
		startColor.R = Lerp::lerp(corner.color.R, left.color.R, progress);
		startColor.G = Lerp::lerp(corner.color.G, left.color.G, progress);
		startColor.B = Lerp::lerp(corner.color.B, left.color.B, progress);

		endColor.R = Lerp::lerp(corner.color.R, right.color.R, progress);
		endColor.G = Lerp::lerp(corner.color.G, right.color.G, progress);
		endColor.B = Lerp::lerp(corner.color.B, right.color.B, progress);

		if (texture != NULL) {
			startUV = Vec2::lerp(corner.uv, left.uv, progress);
			endUV = Vec2::lerp(corner.uv, right.uv, progress);
			startW = Lerp::lerp(corner.w, left.w, progress);
			endW = Lerp::lerp(corner.w, right.w, progress);
		}

		if (lineLength > 0) {
			triangleScanLine(startX, y, lineLength, startColor, endColor, startDepth, endDepth, startUV, endUV, startW, endW, texture);
		}
	}
}

void Rasterizer::flatBottomTriangle(const Vertex2d& top, const Vertex2d& bottomLeft, const Vertex2d& bottomRight, const TextureBuffer* texture) {
	flatTriangle(top, bottomLeft, bottomRight, texture);
}

void Rasterizer::flatTopTriangle(const Vertex2d& topLeft, const Vertex2d& topRight, const Vertex2d& bottom, const TextureBuffer* texture) {
	flatTriangle(bottom, topLeft, topRight, texture);
}

void Rasterizer::line(int x1, int y1, int x2, int y2) {
	bool isOffScreen = (
		max(x1, x2) < 0 ||
		min(x1, x2) >= width ||
		max(y1, y2) < 0 ||
		min(y1, y2) >= height
	);

	if (isOffScreen) {
		return;
	}

	int deltaX = x2 - x1;
	int deltaY = y2 - y1;
	bool isGoingLeft = deltaX < 0;
	bool isGoingUp = deltaY < 0;
	int totalPixels = abs(deltaX) + abs(deltaY);

	for (int i = 0; i < totalPixels; i++) {
		float progress = (float)i / totalPixels;
		int x = x1 + (int)(deltaX * progress);
		int y = y1 + (int)(deltaY * progress);

		bool isGoingOffScreen = (
			(isGoingLeft && x < 0) ||
			(!isGoingLeft && x >= width) ||
			(isGoingUp && y < 0) ||
			(!isGoingUp && y >= height)
		);

		if (isGoingOffScreen) {
			break;
		} else if (x < 0 || x >= width || y < 0 || y >= height) {
			continue;
		}

		setPixel(x, y);
	}
}

void Rasterizer::render(SDL_Renderer* renderer, int sizeFactor = 1) {
	SDL_Rect destinationRect = { 0, 0, sizeFactor * width, sizeFactor * height };

	SDL_UpdateTexture(screenTexture, NULL, pixelBuffer, width * sizeof(Uint32));
	SDL_RenderCopy(renderer, screenTexture, NULL, &destinationRect);
}

void Rasterizer::setBackgroundColor(const Color& color) {
	backgroundColor = ARGB(color.R, color.G, color.B);
}

void Rasterizer::setColor(Uint32 color) {
	this->color = color;
}

void Rasterizer::setColor(int R, int G, int B) {
	color = ARGB(R, G, B);
}

void Rasterizer::setColor(const Color& color) {
	setColor(color.R, color.G, color.B);
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
 * Rasterize a filled triangle with per-vertex coloration or textures.
 */
void Rasterizer::triangle(Triangle& triangle) {
	// Sort each vertex from top to bottom
	Vertex2d* top = &triangle.vertices[0];
	Vertex2d* middle = &triangle.vertices[1];
	Vertex2d* bottom = &triangle.vertices[2];
	const TextureBuffer* texture = triangle.texture;

	if (top->coordinate.y > middle->coordinate.y) swap(top, middle);
	if (middle->coordinate.y > bottom->coordinate.y) swap(middle, bottom);
	if (top->coordinate.y > middle->coordinate.y) swap(top, middle);

	if (top->coordinate.y >= height || bottom->coordinate.y < 0) {
		// Optimize for vertically offscreen triangles
		return;
	}

	if (!shouldUsePerVertexColoration) {
		setColor(triangle.vertices[0].color);
	}

	if (top->coordinate.y == middle->coordinate.y) {
		// Trivial case #1: Triangle with a flat top edge
		if (top->coordinate.x > middle->coordinate.x) {
			swap(top, middle);
		}

		flatTopTriangle(*top, *middle, *bottom, texture);
	} else if (bottom->coordinate.y == middle->coordinate.y) {
		// Trivial case #2: Triangle with a flat bottom edge
		if (bottom->coordinate.x < middle->coordinate.x) {
			swap(bottom, middle);
		}

		flatBottomTriangle(*top, *middle, *bottom, texture);
	} else {
		// Nontrivial case: Triangle with neither a flat top nor
		// flat bottom edge. These must be rasterized as two
		// separate flat-bottom-edge and flat-top-edge triangles.
		float middleYProgress = (float)(middle->coordinate.y - top->coordinate.y) / (bottom->coordinate.y - top->coordinate.y);

		// To rasterize each half of the triangle properly, we must
		// construct an intermediate vertex along its hypotenuse,
		// level with the actual middle vertex.
		Vertex2d hypotenuseVertex = Vertex2d::lerp(*top, *bottom, middleYProgress);

		// Lock the y coordinate of the new vertex to that of the
		// middle vertex to avoid potential lerp rounding errors
		hypotenuseVertex.coordinate.y = middle->coordinate.y;

		Vertex2d* middleLeft = middle;
		Vertex2d* middleRight = &hypotenuseVertex;

		if (middleLeft->coordinate.x > middleRight->coordinate.x) {
			swap(middleLeft, middleRight);
		}

		flatBottomTriangle(*top, *middleLeft, *middleRight, texture);
		flatTopTriangle(*middleLeft, *middleRight, *bottom, texture);
	}
}

/**
 * Rasterizes a single line across a section of a filled triangle.
 * Since this function controls the loop which operates on the level
 * of individual pixels, it is the most performance-critical part
 * of the system, and care must be taken to ensure that it includes
 * no unnecessary work.
 */
void Rasterizer::triangleScanLine(
	int x1, int y1, int lineLength,
	const Color& startColor, const Color& endColor,
	int startDepth, int endDepth,
	const Vec2& startUV, const Vec2& endUV,
	float startW, float endW,
	const TextureBuffer* texture
) {
	int start = FAST_MAX(x1, 0);
	int end = FAST_MIN(x1 + lineLength, width - 1);
	int pixelIndexOffset = y1 * width;

	// Rather than interpolating a new color value at every pixel
	// along the line, we can derive an optimal lerp update interval
	// based on the color change over the line and its length. The
	// use of a counter also improves performance compared to modulo.
	float colorDelta = (abs(endColor.R - startColor.R) + abs(endColor.G - startColor.G) + abs(endColor.B - startColor.B)) / 3;
	int lerpInterval = colorDelta > 0 ? FAST_MAX(1, (int)(lineLength / colorDelta)) : lineLength;
	int lerpIntervalCounter = lerpInterval;

	int R = startColor.R;
	int G = startColor.G;
	int B = startColor.B;

	float depthStep = (float)(endDepth - startDepth) / lineLength;
	float f_depth = (float)startDepth + depthStep * (start - x1);

	for (int x = start; x <= end; x++) {
		int index = pixelIndexOffset + x;
		int depth = (int)f_depth;

		f_depth += depthStep;

		if (depthBuffer[index] > depth) {
			float progress = (float)(x - x1) / lineLength;

			if (shouldUsePerVertexColoration) {
				if (++lerpIntervalCounter > lerpInterval || x == end) {
					// Lerping the color components individually is more
					// efficient than lerping startColor -> endColor and
					// generating a new Color object each time
					R = Lerp::lerp(startColor.R, endColor.R, progress);
					G = Lerp::lerp(startColor.G, endColor.G, progress);
					B = Lerp::lerp(startColor.B, endColor.B, progress);

					setColor(R, G, B);

					lerpIntervalCounter = 0;
				}
			}

			if (texture != NULL) {
				float w = 1 / Lerp::lerp(startW, endW, progress);
				float u = Lerp::lerp(startUV.x, endUV.x, progress) * w;
				float v = Lerp::lerp(startUV.y, endUV.y, progress) * w;

				const Color& tex = texture->sample(u, v);

				setColor(tex);

				// int c_R = FAST_CLAMP(tex.R + R, 0, 255);
				// int c_G = FAST_CLAMP(tex.G + G, 0, 255);
				// int c_B = FAST_CLAMP(tex.B + B, 0, 255);

				// setColor(c_R, c_G, c_B);
			}

			// We refrain from calling setPixel() here to avoid
			// its redunant calculation of the index
			pixelBuffer[index] = color;
			depthBuffer[index] = depth;
		}
	}
}
