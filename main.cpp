#define _CRT_SECURE_NO_WARNINGS

#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <Rasterizer.h>

int lastUpdateTime;
int width = 1200;
int height = 720;

SDL_Window* window;
SDL_Renderer* renderer;

void wait(int ms) {
	int startTime = SDL_GetTicks();

	while ((SDL_GetTicks() - startTime) < ms) {}
}

int main(int argc, char* argv[]) {
	SDL_Init(SDL_INIT_EVERYTHING);

	window = SDL_CreateWindow(
		"HEY ZACK",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		width, height,
		SDL_WINDOW_SHOWN
	);

	renderer = SDL_CreateRenderer(window, -1, 0);

	Rasterizer rasterizer(renderer, width, height);

	bool quit = false;
	int lastStartTime;

	const int totalTriangles = 10000;

	srand(time(NULL));

	Triangle* triangles[totalTriangles];

	for (int t = 0; t < totalTriangles; t++) {
		float ratio = (float)t / totalTriangles;
		float r2 = (1 - ratio) * (1 - ratio) * (1 - ratio);

		int size = 1 + (int)400 * r2;
		int halfSize = 1 + (int)200 * r2;

		Triangle* triangle = new Triangle;

		int x1 = rand() % width, y1 = rand() % height;
		int x2 = x1 + rand() % halfSize, y2 = y1 + rand() % halfSize;
		int x3 = x1 + rand() % size, y3 = y2 + rand() % halfSize;

		triangle->setVertex(0, x1, y1, rand() % 255, rand() % 255, rand() % 255);
		triangle->setVertex(1, x2, y2, rand() % 255, rand() % 255, rand() % 255);
		triangle->setVertex(2, x3, y3, rand() % 255, rand() % 255, rand() % 255);

		triangles[t] = triangle;
	}

	while (!quit) {
		lastStartTime = SDL_GetTicks();

		for (int t = 0; t < totalTriangles; t++) {
			Triangle* tri = triangles[t];
			Color* color = tri->vertices[0]->color;

			// rasterizer.setColor(color);
			// rasterizer.triangle(tri->vertices[0]->x, tri->vertices[0]->y, tri->vertices[1]->x, tri->vertices[1]->y, tri->vertices[2]->x, tri->vertices[2]->y);
			rasterizer.triangle(triangles[t]);
		}

		rasterizer.render(renderer);

		int delta = SDL_GetTicks() - lastStartTime;

		if (delta < 16.67) {
			wait((int)round(16.67 - delta));
		}

		int fullDelta = SDL_GetTicks() - lastStartTime;
		char title[50];

		sprintf(title, "Triangles: %d, FPS: %dfps, Unlocked delta: %dms", totalTriangles, (int)round(60 * 16.67 / fullDelta), delta);

		SDL_SetWindowTitle(window, title);

		SDL_Event event;

		if (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_QUIT:
					quit = true;
					break;
			}
		}
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}