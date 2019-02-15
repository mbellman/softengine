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
	depthBuffer = new float[width * height];
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
	fill(depthBuffer, depthBuffer + bufferLength, 0.0f);
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

		scanline->inverseDepth.start = Lerp::lerp(corner.inverseDepth, left.inverseDepth, progress);
		scanline->inverseDepth.end = Lerp::lerp(corner.inverseDepth, right.inverseDepth, progress);

		scanline->texture = texture;

		if (texture != NULL) {
			scanline->perspectiveUV.start = Vec2::lerp(corner.perspectiveUV, left.perspectiveUV, progress);
			scanline->perspectiveUV.end = Vec2::lerp(corner.perspectiveUV, right.perspectiveUV, progress);

			scanline->textureIntensity.start = Vec3::lerp(corner.textureIntensity, left.textureIntensity, progress);
			scanline->textureIntensity.end = Vec3::lerp(corner.textureIntensity, right.textureIntensity, progress);
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
	float colorDelta = (float)(r_delta + g_delta + b_delta) / 3.0f;

	return colorDelta > 0 ? FAST_MAX(Rasterizer::MIN_COLOR_LERP_INTERVAL, (int)(lineLength / colorDelta)) : lineLength;
}

inline int Rasterizer::getMipmapLevel(float averageDepth) {
	if (flags & DISABLE_MIPMAPPING) {
		return 0;
	}

	float depthMultiple = averageDepth / Rasterizer::MIPMAP_RANGE;

	for (int i = 0; i < 12; i++) {
		if (depthMultiple < LOG2_TABLE[i][0]) {
			return LOG2_TABLE[i - 1][1];
		}
	}

	return 10;
}

int Rasterizer::getTextureSampleInterval(int tex_w, int tex_h, int lineLength, float averageDepth, const Range<Vec2>& perspectiveUV) {
	float u_delta = (float)tex_w * abs(perspectiveUV.end.x - perspectiveUV.start.x) * averageDepth;
	float v_delta = (float)tex_h * abs(perspectiveUV.end.y - perspectiveUV.end.y) * averageDepth;
	float sampleDelta = (u_delta + v_delta) / 2.0f;

	if (sampleDelta > 0) {
		int interval = (int)(lineLength / sampleDelta);

		return FAST_CLAMP(interval, 1, Rasterizer::MAX_TEXTURE_SAMPLE_INTERVAL);
	} else {
		return Rasterizer::MAX_TEXTURE_SAMPLE_INTERVAL;
	}
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

void Rasterizer::setPixel(int x, int y) {
	int index = y * width + x;

	pixelBuffer[index] = drawColor;
}

void Rasterizer::setVisibility(int visibility) {
	this->visibility = visibility;
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
		scanline->inverseDepth,
		scanline->perspectiveUV,
		scanline->textureIntensity,
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
	const Range<float>& inverseDepth,
	const Range<Vec2>& perspectiveUV,
	const Range<Vec3>& textureIntensity,
	const TextureBuffer* texture
) {
	int start = FAST_MAX(x1, 0);
	int end = FAST_MIN(x1 + length, width - 1);
	int pixelIndexOffset = y1 * width;

	float depthStep = (inverseDepth.end - inverseDepth.start) / length;
	float i_depth = inverseDepth.start + (start - x1) * depthStep;

	float progressStep = 1.0f / length;
	float progress = (float)(start - x1) * progressStep;

	if (texture != NULL) {
		Uint32 currentColor = 0;

		float averageDepth = (1.0f / inverseDepth.start + 1.0f / inverseDepth.end) / 2.0f;
		int mipmapLevel = getMipmapLevel(averageDepth);
		int tex_w = texture->mipmapWidth(mipmapLevel);
		int tex_h = texture->mipmapHeight(mipmapLevel);
		int textureSampleInterval = getTextureSampleInterval(tex_w, tex_h, length, averageDepth, perspectiveUV);
		int textureSampleIntervalCounter = textureSampleInterval;

		for (int x = start; x <= end; x++) {
			int index = pixelIndexOffset + x;

			if (depthBuffer[index] < i_depth) {
				if (++textureSampleIntervalCounter > textureSampleInterval) {
					float intensity_R = Lerp::lerp(textureIntensity.start.x, textureIntensity.end.x, progress);
					float intensity_G = Lerp::lerp(textureIntensity.start.y, textureIntensity.end.y, progress);
					float intensity_B = Lerp::lerp(textureIntensity.start.z, textureIntensity.end.z, progress);

					float depth = 1.0f / i_depth;
					float u = Lerp::lerp(perspectiveUV.start.x, perspectiveUV.end.x, progress) * depth;
					float v = Lerp::lerp(perspectiveUV.start.y, perspectiveUV.end.y, progress) * depth;
					const Color& sample = texture->sample(u, v, mipmapLevel);

					float visibilityRatio = FAST_MIN(depth / visibility, 1.0f);

					int R = Lerp::lerp((int)(sample.R * intensity_R), backgroundColor.R, visibilityRatio);
					int G = Lerp::lerp((int)(sample.G * intensity_G), backgroundColor.G, visibilityRatio);
					int B = Lerp::lerp((int)(sample.B * intensity_B), backgroundColor.B, visibilityRatio);

					currentColor = ARGB(FAST_CLAMP(R, 0, 255), FAST_CLAMP(G, 0, 255), FAST_CLAMP(B, 0, 255));
					textureSampleIntervalCounter = 0;
				}

				pixelBuffer[index] = currentColor;
				depthBuffer[index] = i_depth;
			}

			progress += progressStep;
			i_depth += depthStep;
		}
	} else {
		int colorLerpInterval = getColorLerpInterval(color.start, color.end, length);
		int colorLerpIntervalCounter = colorLerpInterval;

		if (colorLerpInterval > 5) {
			int chunkXLimit = end + 1;
			int index = pixelIndexOffset + start;

			progressStep *= colorLerpInterval;

			for (int x = start; x <= end; x += colorLerpInterval) {
				int R = Lerp::lerp(color.start.R, color.end.R, progress);
				int G = Lerp::lerp(color.start.G, color.end.G, progress);
				int B = Lerp::lerp(color.start.B, color.end.B, progress);

				int cx2 = FAST_MIN(x + colorLerpInterval, chunkXLimit);
				Uint32 color = ARGB(R, G, B);

				for (int cx = x; cx < cx2; cx++) {
					if (depthBuffer[index] < i_depth) {
						pixelBuffer[index] = color;
						depthBuffer[index] = i_depth;
					}

					index++;
					i_depth += depthStep;
				}

				progress += progressStep;
			}
		} else {
			Uint32 currentColor = 0;

			for (int x = start; x <= end; x++) {
				int index = pixelIndexOffset + x;

				if (depthBuffer[index] < i_depth) {
					if (++colorLerpIntervalCounter > colorLerpInterval || x == end) {
						int R = Lerp::lerp(color.start.R, color.end.R, progress);
						int G = Lerp::lerp(color.start.G, color.end.G, progress);
						int B = Lerp::lerp(color.start.B, color.end.B, progress);

						currentColor = ARGB(R, G, B);
						colorLerpIntervalCounter = 0;
					}

					pixelBuffer[index] = currentColor;
					depthBuffer[index] = i_depth;
				}

				progress += progressStep;
				i_depth += depthStep;
			}
		}
	}
}
