#include <math.h>
#include <stdio.h>
#include <algorithm>
#include <limits.h>
#include <Helpers.h>
#include <Graphics/Rasterizer.h>
#include <Graphics/TextureBuffer.h>
#include <Objects.h>

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

void Rasterizer::flatTriangle(const Vertex2d& corner, const Vertex2d& left, const Vertex2d& right, TextureBuffer* texture) {
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
	float leftSlope = (float)triangleHeight / (left.coordinate.x - corner.coordinate.x);
	float rightSlope = (float)triangleHeight / (right.coordinate.x - corner.coordinate.x);
	bool hasFlatTop = corner.coordinate.y > left.coordinate.y;

	for (int y = start; y < end; y++) {
		int step = hasFlatTop ? (triangleHeight - (y - topY)) : (y - topY);
		float progress = (float)step / triangleHeight;
		int startX = corner.coordinate.x + (int)step / leftSlope;
		int endX = corner.coordinate.x + (int)step / rightSlope;
		Color startColor = lerp(corner.color, left.color, progress);
		Color endColor = lerp(corner.color, right.color, progress);
		int startDepth = lerp(corner.depth, left.depth, progress);
		int endDepth = lerp(corner.depth, right.depth, progress);
		Vec2 startUV = lerp(corner.uv, left.uv, progress);
		Vec2 endUV = lerp(corner.uv, right.uv, progress);

		triangleScanLine(startX, y, endX - startX, startColor, endColor, startDepth, endDepth, startUV, endUV, texture);
	}
}

void Rasterizer::flatBottomTriangle(
	const Vertex2d& top, const Vertex2d& bottomLeft, const Vertex2d& bottomRight,
	TextureBuffer* texture
) {
	flatTriangle(top, bottomLeft, bottomRight, texture);
}

void Rasterizer::flatTopTriangle(
	const Vertex2d& topLeft, const Vertex2d& topRight, const Vertex2d& bottom,
	TextureBuffer* texture
) {
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
	// Sort each vertex from top to bottom
	Vertex2d* top = &triangle.vertices[0];
	Vertex2d* middle = &triangle.vertices[1];
	Vertex2d* bottom = &triangle.vertices[2];
	TextureBuffer* texture = triangle.parentPolygon->parentObject->texture;

	if (top->coordinate.y > middle->coordinate.y) {
		swap(top, middle);
	}

	if (middle->coordinate.y > bottom->coordinate.y) {
		swap(middle, bottom);
	}

	if (top->coordinate.y > middle->coordinate.y) {
		swap(top, middle);
	}

	if (top->coordinate.y >= height || bottom->coordinate.y < 0) {
		// Optimize for vertically offscreen triangles
		return;
	}

	if (!shouldUsePerVertexColoration) {
		setColor(&triangle.vertices[0].color);
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
		float hypotenuseSlope = (float)(bottom->coordinate.y - top->coordinate.y) / (bottom->coordinate.x - top->coordinate.x);
		float middleYProgress = (float)(middle->coordinate.y - top->coordinate.y) / (bottom->coordinate.y - top->coordinate.y);

		// To rasterize each half of the triangle properly, we must
		// construct an intermediate vertex along its hypotenuse,
		// level with the actual middle vertex. This will serve to
		// help interpolate between the top/bottom vertices along the
		// sliced portions of the hypotenuse for each new triangle.
		Vertex2d hypotenuseVertex;
		int x = top->coordinate.x + (int)((middle->coordinate.y - top->coordinate.y) / hypotenuseSlope);
		int y = middle->coordinate.y;

		hypotenuseVertex.coordinate = { x, y };
		hypotenuseVertex.depth = lerp(top->depth, bottom->depth, middleYProgress);
		hypotenuseVertex.color = lerp(top->color, bottom->color, middleYProgress);
		hypotenuseVertex.uv = lerp(top->uv, bottom->uv, middleYProgress);

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
	TextureBuffer* texture
) {
	if (y1 >= height || y1 < 0 || lineLength == 0) {
		// Optimize for vertically offscreen lines or zero-length
		// lines. Most horizontally offscreen lines are automatically
		// avoided by preemptively checking the left and right edges
		// of the triangle in Rasterizer::flatTriangle(), and not
		// drawing it if its entire boundary is offscreen. Otherwise,
		// horizontally offscreen but vertically onscreen lines are
		// unlikely to be a problem for visible triangles.
		return;
	}

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

	// There is a small but appreciable performance improvement
	// when incrementing a floating-point depth step on each cycle
	// to determine the new per-pixel depth, rather than lerping
	// startDepth -> endDepth every time. (This technique has
	// little effect when used to update the color components,
	// since the lerp interval reduces the number of actual color
	// lerps by a significant degree.)
	float depthStep = (float)(endDepth - startDepth) / lineLength;
	float f_depth = (float)startDepth + depthStep * (start - x1);

	for (int x = start; x <= end; x++) {
		int index = pixelIndexOffset + x;
		int depth = (int)f_depth;

		f_depth += depthStep;

		if (depthBuffer[index] > depth) {
			if (texture == NULL && shouldUsePerVertexColoration) {
				if (++lerpIntervalCounter > lerpInterval || x == end) {
					float progress = (float)(x - x1) / lineLength;

					// Lerping the color components individually is more
					// efficient than lerping startColor -> endColor and
					// generating a new Color object each time
					int R = lerp(startColor.R, endColor.R, progress);
					int G = lerp(startColor.G, endColor.G, progress);
					int B = lerp(startColor.B, endColor.B, progress);

					setColor(R, G, B);

					lerpIntervalCounter = 0;
				}
			} else if (texture != NULL) {
				float progress = (float)(x - x1) / lineLength;
				int u = (int)(lerp(startUV.x, endUV.x, progress) * texture->width);
				int v = (int)(lerp(startUV.y, endUV.y, progress) * texture->height);

				setColor(texture->sample(u, v));
			}

			// We refrain from calling setPixel() here to avoid
			// its redunant calculation of the index
			pixelBuffer[index] = color;
			depthBuffer[index] = depth;
		}
	}
}
