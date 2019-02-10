#pragma once

#include <SDL.h>
#include <limits.h>
#include <System/Math.h>
#include <System/Geometry.h>
#include <Graphics/TextureBuffer.h>

/**
 * Rasterizer
 * ----------
 */
class Rasterizer {
public:
	Rasterizer(SDL_Renderer* renderer, int width, int height, bool shouldUsePerVertexColoration);
	~Rasterizer();

	void clear();
	void line(int x1, int y1, int x2, int y2);
	void render(SDL_Renderer* renderer, int sizeFactor);
	void setBackgroundColor(const Color& color);
	void setColor(int R, int G, int B);
	void setColor(const Color& color);
	void setColor(Uint32 color);
	void setTextureIntensity(float intensity);
	void triangle(int x1, int y1, int x2, int y2, int x3, int y3);
	void triangle(Triangle& triangle);

private:
	constexpr static int MIN_COLOR_LERP_INTERVAL = 2;
	constexpr static int MAX_TEXTURE_SAMPLE_INTERVAL = 5;

	Uint32 backgroundColor = 0;
	SDL_Texture* screenTexture;
	Uint32* pixelBuffer;
	int* depthBuffer;
	bool shouldUsePerVertexColoration = true;
	Uint32 color;
	int width;
	int height;
	Color textureColorReduction = { 0, 0, 0 };

	void flatTriangle(const Vertex2d& corner, const Vertex2d& left, const Vertex2d& right, const TextureBuffer* texture);
	void flatBottomTriangle(const Vertex2d& top, const Vertex2d& bottomLeft, const Vertex2d& bottomRight, const TextureBuffer* texture);
	void flatTopTriangle(const Vertex2d& topLeft, const Vertex2d& topRight, const Vertex2d& bottom, const TextureBuffer* texture);
	int getColorLerpInterval(const Color& start, const Color& end, int lineLength);
	int getTextureSampleInterval(const TextureBuffer* texture, int lineLength, const Vec2& startUV, const Vec2& endUV, int startDepth, int endDepth);

	void triangleScanLine(
		int x1, int y1, int width,
		const Color& startColor, const Color& endColor,
		int startDepth, int endDepth,
		const Vec2& startUV, const Vec2& endUV,
		float startW, float endW,
		const TextureBuffer* texture
	);

	void triangleScanLineChunk(int x, int y, int width, Uint32 color, int depth, int offset);
	void setPixel(int x, int y, int depth = 1);
};
