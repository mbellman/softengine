#pragma once

#include <SDL.h>
#include <limits.h>
#include <Types.h>
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
	void setColor(Color* color);
	void triangle(int x1, int y1, int x2, int y2, int x3, int y3);
	void triangle(Triangle& triangle);

private:
	Uint32 backgroundColor = 0;
	SDL_Texture* screenTexture;
	Uint32* pixelBuffer;
	int* depthBuffer;
	bool shouldUsePerVertexColoration = true;
	long int color;
	int width;
	int height;
	void flatTriangle(const Vertex2d& corner, const Vertex2d& left, const Vertex2d& right);
	void flatBottomTriangle(const Vertex2d& top, const Vertex2d& bottomLeft, const Vertex2d& bottomRight);
	void flatTopTriangle(const Vertex2d& topLeft, const Vertex2d& topRight, const Vertex2d& bottom);
	void triangleScanLine(int x1, int y1, int width, const Color& startColor, const Color& endColor, int leftDepth, int rightDepth);
	void setPixel(int x, int y, int depth = 1);
	Uint32 rgbToUint32(int R, int G, int B);
};
