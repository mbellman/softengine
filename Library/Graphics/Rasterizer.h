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
	Rasterizer(SDL_Renderer* renderer, int width, int height, Uint32 flags = 0);
	~Rasterizer();

	void clear();
	int getTotalScanlines();
	void line(int x1, int y1, int x2, int y2);
	void render(SDL_Renderer* renderer, int sizeFactor);
	void setBackgroundColor(const Color& color);
	void setDrawColor(int R, int G, int B);
	void setDrawColor(const Color& color);
	void setDrawColor(Uint32 color);
	void setVisibility(int visibility);
	void triangle(int x1, int y1, int x2, int y2, int x3, int y3);
	void triangle(Triangle& triangle);

private:
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

	struct ScanlineThreadManager {
		Rasterizer* rasterizer;
		int section;
		bool isDone = true;
	};

	constexpr static int MIN_COLOR_LERP_INTERVAL = 2;
	constexpr static int MAX_TEXTURE_SAMPLE_INTERVAL = 4;
	constexpr static int MAX_VISIBILITY = INT_MAX;
	constexpr static float MIPMAP_RANGE = 800.0f;

	constexpr static int LOG2_TABLE[12][2] = {
		{ 0, 0 },
		{ 1, 0 },
		{ 2, 1 },
		{ 4, 2 },
		{ 8, 3 },
		{ 16, 4 },
		{ 32, 5 },
		{ 64, 6 },
		{ 128, 7 },
		{ 256, 8 },
		{ 512, 9 },
		{ 1024, 10 }
	};

	Scanline* scanlines;
	ScanlineThreadManager* scanlineThreadManagers;
	int totalBufferedScanlines = 0;
	std::vector<SDL_Thread*> scanlineThreads;
	bool isDone = false;

	Uint32 flags = 0;
	Color backgroundColor = { 0, 0, 0 };
	Uint32 drawColor = ARGB(255, 255, 255);
	int visibility = Rasterizer::MAX_VISIBILITY;
	SDL_Texture* screenTexture;
	Uint32* pixelBuffer;
	float* depthBuffer;
	int width;
	int height;

	static int manageScanlineThread(void* data);
	void createScanlineThreads();
	void flatTriangle(const Vertex2d& corner, const Vertex2d& left, const Vertex2d& right, const TextureBuffer* texture);
	void flatBottomTriangle(const Vertex2d& top, const Vertex2d& bottomLeft, const Vertex2d& bottomRight, const TextureBuffer* texture);
	void flatTopTriangle(const Vertex2d& topLeft, const Vertex2d& topRight, const Vertex2d& bottom, const TextureBuffer* texture);
	void flushScanlines();
	int getColorLerpInterval(const Color& start, const Color& end, int lineLength);
	inline int getMipmapLevel(float averageDepth);
	int getTextureSampleInterval(int tex_w, int tex_h, int lineLength, float averageDepth, const Range<Vec2>& perspectiveUV);
	void setPixel(int x, int y);
	void triangleScanline(Scanline* scanline);

	void triangleScanline(
		int x1, int y1, int length,
		const Range<Color>& color,
		const Range<float>& inverseDepth,
		const Range<Vec2>& perspectiveUV,
		const Range<Vec3>& textureIntensity,
		const TextureBuffer* texture
	);
};
