#pragma once

#include <SDL.h>
#include <vector>
#include <limits.h>
#include <System/Math.h>
#include <System/Geometry.h>
#include <Helpers.h>
#include <Graphics/TextureBuffer.h>

/**
 * Rasterizer
 * ----------
 */
class Rasterizer {
public:
	Rasterizer(SDL_Renderer* renderer, int width, int height, bool shouldAllowMultithreading);
	~Rasterizer();

	void clear();
	void flushScanlines();
	void line(int x1, int y1, int x2, int y2);
	void render(SDL_Renderer* renderer, int sizeFactor);
	void setBackgroundColor(const Color& color);
	void setDrawColor(int R, int G, int B);
	void setDrawColor(const Color& color);
	void setDrawColor(Uint32 color);
	void triangle(int x1, int y1, int x2, int y2, int x3, int y3);
	void triangle(Triangle& triangle);

private:
	struct Scanline {
		int x = 0;
		int y = 0;
		int length = 0;
		Range<Color> color;
		Range<int> depth;
		Range<Vec2> uv;
		Range<float> w;
		Range<float> textureLuminosity;
		Range<float> visibility;
		const TextureBuffer* texture;
	};

	struct ScanlineThreadManager {
		Rasterizer* rasterizer;
		int section;
		bool isDone = true;
	};

	constexpr static int MIN_COLOR_LERP_INTERVAL = 2;
	constexpr static int MAX_TEXTURE_SAMPLE_INTERVAL = 4;
	constexpr static int MAX_THREADS = 8;

	Scanline* scanlines;
	ScanlineThreadManager* scanlineThreadManagers;
	int totalBufferedScanlines = 0;
	std::vector<SDL_Thread*> scanlineThreads;
	bool isDone = false;

	Uint32 backgroundColor = 0;
	Uint32 drawColor = ARGB(255, 255, 255);
	SDL_Texture* screenTexture;
	Uint32* pixelBuffer;
	int* depthBuffer;
	int width;
	int height;

	static int manageScanlineThread(void* data);
	void createScanlineThreads();
	void flatTriangle(const Vertex2d& corner, const Vertex2d& left, const Vertex2d& right, const TextureBuffer* texture);
	void flatBottomTriangle(const Vertex2d& top, const Vertex2d& bottomLeft, const Vertex2d& bottomRight, const TextureBuffer* texture);
	void flatTopTriangle(const Vertex2d& topLeft, const Vertex2d& topRight, const Vertex2d& bottom, const TextureBuffer* texture);
	int getColorLerpInterval(const Color& start, const Color& end, int lineLength);
	int getTextureSampleInterval(const TextureBuffer* texture, int lineLength, const Vec2& startUV, const Vec2& endUV, int startDepth, int endDepth);

	void triangleScanline(
		int x1, int y1, int length,
		const Range<Color>& color,
		const Range<int>& depth,
		const Range<Vec2>& uv,
		const Range<float>& w,
		const Range<float>& textureLuminosity,
		const Range<float>& drawDistanceRatio,
		const TextureBuffer* texture
	);

	void triangleScanline(Scanline* scanline);
	void triangleScanlineChunk(int x, int y, int length, Uint32 color, int depth, int offset);
	void setPixel(int x, int y, int depth = 1);
};
