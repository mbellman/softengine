#include <math.h>
#include <stdio.h>
#include <algorithm>
#include <limits.h>
#include <Helpers.h>
#include <Graphics/Rasterizer.h>

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

	std::fill(pixelBuffer, pixelBuffer + bufferLength, 0);
	std::fill(depthBuffer, depthBuffer + bufferLength, INT_MAX);
}

void Rasterizer::flatTriangle(const Vertex2d& corner, const Vertex2d& left, const Vertex2d& right) {
	int isHorizontallyOffscreen = (
		(corner.coordinate.x >= width && left.coordinate.x >= width) ||
		(corner.coordinate.x < 0 && right.coordinate.x < 0)
	);

	if (isHorizontallyOffscreen) {
		return;
	}

	int triangleHeight = std::abs(left.coordinate.y - corner.coordinate.y);
	int topY = std::min(corner.coordinate.y, left.coordinate.y);
	float leftSlope = (float)triangleHeight / (left.coordinate.x - corner.coordinate.x);
	float rightSlope = (float)triangleHeight / (right.coordinate.x - corner.coordinate.x);
	bool hasFlatTop = corner.coordinate.y > left.coordinate.y;
	int i = topY < 0 ? -topY : 0;

	while (i < triangleHeight) {
		int y = topY + i;

		if (y >= height) {
			break;
		}

		int j = hasFlatTop ? triangleHeight - i : i;
		float progress = (float)j / triangleHeight;
		int startX = corner.coordinate.x + (int)j / leftSlope;
		int endX = corner.coordinate.x + (int)j / rightSlope;
		Color leftColor = lerp(corner.color, left.color, progress);
		Color rightColor = lerp(corner.color, right.color, progress);
		int leftDepth = lerp(corner.depth, left.depth, progress);
		int rightDepth = lerp(corner.depth, right.depth, progress);

		triangleScanLine(startX, y, endX - startX, leftColor, rightColor, leftDepth, rightDepth);

		i++;
	}
}

void Rasterizer::flatBottomTriangle(const Vertex2d& top, const Vertex2d& bottomLeft, const Vertex2d& bottomRight) {
	flatTriangle(top, bottomLeft, bottomRight);
}

void Rasterizer::flatTopTriangle(const Vertex2d& topLeft, const Vertex2d& topRight, const Vertex2d& bottom) {
	flatTriangle(bottom, topLeft, topRight);
}

void Rasterizer::line(int x1, int y1, int x2, int y2) {
	bool isOffScreen = (
		std::max(x1, x2) < 0 ||
		std::min(x1, x2) >= width ||
		std::max(y1, y2) < 0 ||
		std::min(y1, y2) >= height
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
	// Sort each vertex from top to bottom
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
			std::swap(top, middle);
		}

		flatTopTriangle(*top, *middle, *bottom);
	} else if (bottom->coordinate.y == middle->coordinate.y) {
		// Trivial case #2: Triangle with a flat bottom edge
		if (bottom->coordinate.x < middle->coordinate.x) {
			std::swap(bottom, middle);
		}

		flatBottomTriangle(*top, *middle, *bottom);
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

		Vertex2d* middleLeft = middle;
		Vertex2d* middleRight = &hypotenuseVertex;

		if (middleLeft->coordinate.x > middleRight->coordinate.x) {
			std::swap(middleLeft, middleRight);
		}

		flatBottomTriangle(*top, *middleLeft, *middleRight);
		flatTopTriangle(*middleLeft, *middleRight, *bottom);
	}
}

/**
 * Rasterizes a single line across a section of a filled triangle.
 * Since this function controls the loop which operates on the level
 * of individual pixels, it is the most performance-critical part
 * of the system, and care must be taken to ensure that it includes
 * no unnecessary work.
 */
void Rasterizer::triangleScanLine(int x1, int y1, int lineLength, const Color& leftColor, const Color& rightColor, int leftDepth, int rightDepth) {
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

	using namespace std;

	int start = max(x1, 0);
	int end = min(x1 + lineLength, width - 1);
	int pixelIndexOffset = y1 * width;

	// Rather than interpolating a new color value at every pixel
	// along the line, we can derive an optimal lerp update interval
	// based on the color change over the line and its length. The
	// use of a counter also improves performance compared to modulo.
	float averageColorDelta = (abs(rightColor.R - leftColor.R) + abs(rightColor.G - leftColor.G) + abs(rightColor.B - leftColor.B)) / 3;
	int lerpInterval = max(1, (int)(lineLength / averageColorDelta));
	int lerpIntervalCounter = lerpInterval;

	for (int x = start; x <= end; x++) {
		float progress = (float)(x - x1) / lineLength;
		int depth = lerp(leftDepth, rightDepth, progress);
		int index = pixelIndexOffset + x;

		if (depthBuffer[index] > depth) {
			if (shouldUsePerVertexColoration) {
				if (++lerpIntervalCounter > lerpInterval || x == end) {
					// Lerping the color components individually is more
					// efficient than lerping leftColor -> rightColor and
					// generating a new Color object each time
					int R = lerp(leftColor.R, rightColor.R, progress);
					int G = lerp(leftColor.G, rightColor.G, progress);
					int B = lerp(leftColor.B, rightColor.B, progress);

					setColor(R, G, B);

					lerpIntervalCounter = 0;
				}
			}

			// We refrain from calling setPixel() here to avoid
			// its additional redunant calculation of the index
			pixelBuffer[index] = color;
			depthBuffer[index] = depth;
		}
	}
}
