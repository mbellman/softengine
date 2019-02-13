#include <Graphics/Rasterizer.h>
#include <math.h>
#include <stdio.h>
#include <algorithm>
#include <limits.h>
#include <Helpers.h>
#include <Graphics/TextureBuffer.h>
#include <System/Geometry.h>
#include <System/Objects.h>
#include <System/Flags.h>

using namespace std;

/**
 * Rasterizer
 * ----------
 */
Rasterizer::Rasterizer(SDL_Renderer* renderer, int width, int height, Uint32 flags) {
	this->width = width;
	this->height = height;
	this->flags = flags;

	screenTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, width, height);
	pixelBuffer = new Uint32[width * height];
	depthBuffer = new int[width * height];
	scanlines = new Scanline[width * height];

	clear();

	if (~flags & DISABLE_MULTITHREADING) {
		createScanlineThreads();
	}
}

Rasterizer::~Rasterizer() {
	isDone = true;

	for (int i = 0; i < scanlineThreads.size(); i++) {
		SDL_WaitThread(scanlineThreads.at(i), NULL);
	}

	SDL_DestroyTexture(screenTexture);

	delete[] pixelBuffer;
	delete[] depthBuffer;
	delete[] scanlines;
}

void Rasterizer::createScanlineThreads() {
	int totalThreads = std::min(Rasterizer::MAX_THREADS, SDL_GetCPUCount());

	if (totalThreads < 2) {
		return;
	}

	scanlineThreadManagers = new ScanlineThreadManager[totalThreads];

	for (int i = 0; i < totalThreads; i++) {
		ScanlineThreadManager* manager = &scanlineThreadManagers[i];

		manager->rasterizer = this;
		manager->section = i;

		SDL_Thread* thread = SDL_CreateThread(manageScanlineThread, NULL, manager);
		scanlineThreads.push_back(thread);
	}
}

void Rasterizer::clear() {
	int bufferLength = width * height;
	Uint32 clearColor = ARGB(backgroundColor.R, backgroundColor.G, backgroundColor.B);

	totalBufferedScanlines = 0;

	fill(pixelBuffer, pixelBuffer + bufferLength, clearColor);
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

	for (int y = start; y < end; y++) {
		int step = hasFlatTop ? (triangleHeight - (y - topY)) : (y - topY);
		float progress = (float)step / triangleHeight;
		int startX = Lerp::lerp(corner.coordinate.x, left.coordinate.x, progress);
		int endX = Lerp::lerp(corner.coordinate.x, right.coordinate.x, progress);
		int length = endX - startX;

		if (length == 0) {
			continue;
		}

		Scanline* scanline = &scanlines[totalBufferedScanlines++];

		scanline->x = startX;
		scanline->y = y;
		scanline->length = length;
		scanline->depth.start = Lerp::lerp(corner.depth, left.depth, progress);
		scanline->depth.end = Lerp::lerp(corner.depth, right.depth, progress);

		scanline->texture = texture;

		if (texture != NULL) {
			scanline->uv.start = Vec2::lerp(corner.uv, left.uv, progress);
			scanline->uv.end = Vec2::lerp(corner.uv, right.uv, progress);

			scanline->w.start = Lerp::lerp(corner.w, left.w, progress);
			scanline->w.end = Lerp::lerp(corner.w, right.w, progress);

			scanline->textureIntensity.start = Vec3::lerp(corner.textureIntensity, left.textureIntensity, progress);
			scanline->textureIntensity.end = Vec3::lerp(corner.textureIntensity, right.textureIntensity, progress);

			scanline->visibility.start = Lerp::lerp(corner.visibility, left.visibility, progress);
			scanline->visibility.end = Lerp::lerp(corner.visibility, right.visibility, progress);
		} else {
			scanline->color.start.R = Lerp::lerp(corner.color.R, left.color.R, progress);
			scanline->color.start.G = Lerp::lerp(corner.color.G, left.color.G, progress);
			scanline->color.start.B = Lerp::lerp(corner.color.B, left.color.B, progress);

			scanline->color.end.R = Lerp::lerp(corner.color.R, right.color.R, progress);
			scanline->color.end.G = Lerp::lerp(corner.color.G, right.color.G, progress);
			scanline->color.end.B = Lerp::lerp(corner.color.B, right.color.B, progress);
		}
	}
}

void Rasterizer::flatBottomTriangle(const Vertex2d& top, const Vertex2d& bottomLeft, const Vertex2d& bottomRight, const TextureBuffer* texture) {
	flatTriangle(top, bottomLeft, bottomRight, texture);
}

void Rasterizer::flatTopTriangle(const Vertex2d& topLeft, const Vertex2d& topRight, const Vertex2d& bottom, const TextureBuffer* texture) {
	flatTriangle(bottom, topLeft, topRight, texture);
}

void Rasterizer::flushScanlines() {
	if (scanlineThreads.size() == 0) {
		for (int i = 0; i < totalBufferedScanlines; i++) {
			triangleScanline(&scanlines[i]);
		}
	} else {
		for (int i = 0; i < scanlineThreads.size(); i++) {
			scanlineThreadManagers[i].isDone = false;
		}

		for (int i = 0; i < scanlineThreads.size(); i++) {
			while (!scanlineThreadManagers[i].isDone) {
				SDL_Delay(1);
			}
		}
	}
}

int Rasterizer::getColorLerpInterval(const Color& start, const Color& end, int lineLength) {
	int r_delta = abs(end.R - start.R);
	int g_delta = abs(end.G - start.G);
	int b_delta = abs(end.B - start.B);
	float colorDelta = (r_delta + g_delta + b_delta) / 3;

	return colorDelta > 0 ? FAST_MAX(Rasterizer::MIN_COLOR_LERP_INTERVAL, (int)(lineLength / colorDelta)) : lineLength;
}

inline int Rasterizer::getMipmapLevel(int depth) {
	return (flags & DISABLE_MIPMAPPING) ? 0 : (int)(depth / 1000.0f);
}

int Rasterizer::getTextureSampleInterval(const TextureBuffer* texture, int lineLength, const Vec2& startUV, const Vec2& endUV, int startDepth, int endDepth) {
	float averageDepth = (float)(startDepth + endDepth) / 2.0f;
	float u_delta = (float)texture->width * abs(endUV.x - startUV.x) * averageDepth * 4;
	float v_delta = (float)texture->height * abs(endUV.y - startUV.y) * averageDepth * 4;
	float sampleDelta = (u_delta + v_delta) / 2;
	int interval = (int)(1 + lineLength / sampleDelta);

	return sampleDelta > 0 ? FAST_CLAMP(interval, 1, Rasterizer::MAX_TEXTURE_SAMPLE_INTERVAL) : lineLength;
}

int Rasterizer::manageScanlineThread(void* data) {
	ScanlineThreadManager* manager = (ScanlineThreadManager*)data;
	Rasterizer* rasterizer = manager->rasterizer;

	while (1) {
		if (rasterizer->isDone) {
			break;
		} else if (!manager->isDone) {
			int totalSections = rasterizer->scanlineThreads.size();

			for (int i = 0; i < rasterizer->totalBufferedScanlines; i++) {
				Scanline* scanline = &rasterizer->scanlines[i];

				if (scanline->y % totalSections == manager->section) {
					rasterizer->triangleScanline(scanline);
				}
			}

			manager->isDone = true;
		}

		SDL_Delay(1);
	}

	return 0;
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

	flushScanlines();
	SDL_UpdateTexture(screenTexture, NULL, pixelBuffer, width * sizeof(Uint32));
	SDL_RenderCopy(renderer, screenTexture, NULL, &destinationRect);
}

void Rasterizer::setBackgroundColor(const Color& color) {
	backgroundColor.R = color.R;
	backgroundColor.G = color.G;
	backgroundColor.B = color.B;
}

void Rasterizer::setDrawColor(Uint32 color) {
	drawColor = color;
}

void Rasterizer::setDrawColor(int R, int G, int B) {
	drawColor = ARGB(R, G, B);
}

void Rasterizer::setDrawColor(const Color& color) {
	setDrawColor(color.R, color.G, color.B);
}

void Rasterizer::setPixel(int x, int y, int depth) {
	int index = y * width + x;

	pixelBuffer[index] = drawColor;
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

void Rasterizer::triangleScanline(Scanline* scanline) {
	triangleScanline(
		scanline->x, scanline->y, scanline->length,
		scanline->color,
		scanline->depth,
		scanline->uv,
		scanline->w,
		scanline->textureIntensity,
		scanline->visibility,
		scanline->texture
	);
}

/**
 * Rasterizes a single line across a section of a filled triangle.
 * Since this function controls the loop which operates on the level
 * of individual pixels, it is the most performance-critical part
 * of the system, and care must be taken to ensure that it includes
 * no unnecessary work.
 */
void Rasterizer::triangleScanline(
	int x1, int y1, int length,
	const Range<Color>& color,
	const Range<int>& depth,
	const Range<Vec2>& uv,
	const Range<float>& w,
	const Range<Vec3>& textureIntensity,
	const Range<float>& visibility,
	const TextureBuffer* texture
) {
	int start = FAST_MAX(x1, 0);
	int end = FAST_MIN(x1 + length, width - 1);
	int pixelIndexOffset = y1 * width;

	int colorLerpInterval = getColorLerpInterval(color.start, color.end, length);
	int colorLerpIntervalCounter = colorLerpInterval;

	float depthStep = (float)(depth.end - depth.start) / length;
	float f_depth = (float)depth.start + depthStep * (start - x1);

	if (texture == NULL && colorLerpInterval > 5) {
		int max_x = end + 1;

		depthStep *= colorLerpInterval;

		for (int x = start; x <= end; x += colorLerpInterval) {
			int index = pixelIndexOffset + x;
			float progress = (float)(x - x1) / length;
			int x2 = FAST_MIN(x + colorLerpInterval, max_x);

			int R = Lerp::lerp(color.start.R, color.end.R, progress);
			int G = Lerp::lerp(color.start.G, color.end.G, progress);
			int B = Lerp::lerp(color.start.B, color.end.B, progress);

			triangleScanlineChunk(x, x2, y1, ARGB(R, G, B), (int)f_depth, index);

			f_depth += depthStep;
		}
	} else if (texture == NULL) {
		Uint32 currentColor = 0;

		for (int x = start; x <= end; x++) {
			int index = pixelIndexOffset + x;
			int depth = (int)f_depth;

			f_depth += depthStep;

			if (depthBuffer[index] > depth) {
				float progress = (float)(x - x1) / length;

				if (++colorLerpIntervalCounter > colorLerpInterval || x == end) {
					int R = Lerp::lerp(color.start.R, color.end.R, progress);
					int G = Lerp::lerp(color.start.G, color.end.G, progress);
					int B = Lerp::lerp(color.start.B, color.end.B, progress);

					currentColor = ARGB(R, G, B);
					colorLerpIntervalCounter = 0;
				}

				pixelBuffer[index] = currentColor;
				depthBuffer[index] = depth;
			}
		}
	} else {
		Uint32 currentColor = 0;

		int textureSampleInterval = getTextureSampleInterval(texture, length, uv.start, uv.end, depth.start, depth.end);
		int textureSampleIntervalCounter = textureSampleInterval;

		for (int x = start; x <= end; x++) {
			int index = pixelIndexOffset + x;
			int depth = (int)f_depth;

			f_depth += depthStep;

			if (depthBuffer[index] > depth) {
				if (++textureSampleIntervalCounter > textureSampleInterval) {
					float progress = (float)(x - x1) / length;

					float intensity_R = Lerp::lerp(textureIntensity.start.x, textureIntensity.end.x, progress);
					float intensity_G = Lerp::lerp(textureIntensity.start.y, textureIntensity.end.y, progress);
					float intensity_B = Lerp::lerp(textureIntensity.start.z, textureIntensity.end.z, progress);

					float i_w = 1 / Lerp::lerp(w.start, w.end, progress);
					float u = Lerp::lerp(uv.start.x, uv.end.x, progress) * i_w;
					float v = Lerp::lerp(uv.start.y, uv.end.y, progress) * i_w;
					const Color& sample = texture->sample(u, v, getMipmapLevel(depth));

					float vis = Lerp::lerp(visibility.start, visibility.end, progress);

					int R = Lerp::lerp(backgroundColor.R, (int)(sample.R * intensity_R), vis);
					int G = Lerp::lerp(backgroundColor.G, (int)(sample.G * intensity_G), vis);
					int B = Lerp::lerp(backgroundColor.B, (int)(sample.B * intensity_B), vis);

					currentColor = ARGB(FAST_CLAMP(R, 0, 255), FAST_CLAMP(G, 0, 255), FAST_CLAMP(B, 0, 255));
					textureSampleIntervalCounter = 0;
				}

				pixelBuffer[index] = currentColor;
				depthBuffer[index] = depth;
			}
		}
	}
}

void Rasterizer::triangleScanlineChunk(int x1, int x2, int y, Uint32 color, int depth, int offset) {
	for (int x = x1; x < x2; x++) {
		if (depthBuffer[offset] > depth) {
			pixelBuffer[offset] = color;
			depthBuffer[offset] = depth;
		}

		offset++;
	}
}
