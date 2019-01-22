#pragma once

#include <SDL.h>
#include <Types.h>

class Rasterizer {
	public:
		Rasterizer(SDL_Renderer* renderer, int width, int height);
		~Rasterizer();
		void line(int x1, int y1, int x2, int y2);
		void render(SDL_Renderer* renderer);
		void setColor(int R, int G, int B, int A = 255);
		void setColor(Color* color);
		void triangle(int x1, int y1, int x2, int y2, int x3, int y3);
		void triangle(Polygon2d* triangle);
	private:
		SDL_Texture* screenTexture;
		Uint32* pixelBuffer;
		int* depthBuffer;
		long int color;
		int width;
		int height;
		void clear();
		void setPixel(int x, int y, int depth = 1);
};