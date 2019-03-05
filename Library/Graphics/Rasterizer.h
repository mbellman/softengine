#pragma once

#include <SDL.h>
#include <vector>
#include <limits.h>
#include <System/Math.h>
#include <System/Geometry.h>
#include <Helpers.h>
#include <Graphics/TextureBuffer.h>
#include <Constants.h>

/**
 * Scanline
 * --------
 */
struct Scanline {
	int x = 0;
	int y = 0;
	int length = 0;
	Range<Color> color;
	Range<float> inverseDepth;
	Range<Vec2> perspectiveUV;
	Range<Vec3> textureIntensity;
	const TextureBuffer* texture;
};

/**
 * Rasterizer
 * ----------
 */
class Rasterizer {
public:
	Rasterizer(SDL_Renderer* renderer, int width, int height, Uint32 flags = 0);
	~Rasterizer();

	void clear();
	void dispatchTriangle(Triangle& triangle);
	int getTotalBufferedScanlines();
	const Scanline* getScanline(int index);
	void line(int x1, int y1, int x2, int y2);
	void render(SDL_Renderer* renderer, int sizeFactor);
	void setBackgroundColor(const Color& color);
	void setDrawColor(int R, int G, int B);
	void setDrawColor(const Color& color);
	void setDrawColor(Uint32 color);
	void setVisibility(int visibility);
	void triangle(int x1, int y1, int x2, int y2, int x3, int y3);
	void triangleScanline(const Scanline* scanline);

private:
	Scanline* scanlines;
	int totalBufferedScanlines = 0;
	Uint32 flags = 0;
	Color backgroundColor = { 0, 0, 0 };
	Uint32 drawColor = ARGB(255, 255, 255);
	int visibility = MAX_VISIBILITY;
	SDL_Texture* screenTexture;
	Uint32* pixelBuffer;
	float* depthBuffer;
	int bufferSize;
	int width;
	int height;

	void dispatchFlatTriangle(const Vertex2d& corner, const Vertex2d& left, const Vertex2d& right, const TextureBuffer* texture);
	void dispatchFlatBottomTriangle(const Vertex2d& top, const Vertex2d& bottomLeft, const Vertex2d& bottomRight, const TextureBuffer* texture);
	void dispatchFlatTopTriangle(const Vertex2d& topLeft, const Vertex2d& topRight, const Vertex2d& bottom, const TextureBuffer* texture);
	void flushScanlines();
	int getColorLerpInterval(const Color& start, const Color& end, int lineLength);
	int getMipmapLevel(float averageDepth);
	int getTextureSampleInterval(int lineLength, float averageDepth);
	void setPixel(int x, int y);

	void triangleScanline(
		int x1, int y1, int length,
		const Range<Color>& color,
		const Range<float>& inverseDepth,
		const Range<Vec2>& perspectiveUV,
		const Range<Vec3>& textureIntensity,
		const TextureBuffer* texture
	);
};
