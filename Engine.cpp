#define _CRT_SECURE_NO_WARNINGS

#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <Objects.h>
#include <Engine.h>

Engine::Engine() {
	SDL_Init(SDL_INIT_EVERYTHING);
}

Engine::~Engine() {
	objects.clear();
	delete rasterizer;

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void Engine::addObject(Object* object) {
	objects.push_back(object);
}

void Engine::delay(int ms) {
	int startTime = SDL_GetTicks();

	while ((SDL_GetTicks() - startTime) < ms) {}
}

void Engine::draw() {
	rasterizer->setColor(255, 0, 0);

	for (int o = 0; o < objects.size(); o++) {
		objects.at(0)->forEachPolygon([=](const Vertex3D& v1, const Vertex3D& v2, const Vertex3D& v3) {
			rasterizer->triangle(v1.x, v1.z, v2.x, v2.z, v3.x, v3.z);
		});
	}

	rasterizer->render(renderer);
}

int Engine::getPolygonCount() {
	int total = 0;

	for (int o = 0; o < objects.size(); o++) {
		total += objects.at(o)->getPolygonCount();
	}

	return total;
}

void Engine::run(int width, int height) {
	window = SDL_CreateWindow(
		"HEY ZACK",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		width, height,
		SDL_WINDOW_SHOWN
	);

	renderer = SDL_CreateRenderer(window, -1, 0);
	rasterizer = new Rasterizer(renderer, width, height);

	int lastStartTime;

	while (true) {
		lastStartTime = SDL_GetTicks();

		draw();

		int delta = SDL_GetTicks() - lastStartTime;

		if (delta < 16.67) {
			delay((int)round(16.67 - delta));
		}

		int fullDelta = SDL_GetTicks() - lastStartTime;
		char title[100];

		sprintf(title, "Objects: %d, Polygons: %d, FPS: %dfps, Unlocked delta: %dms", objects.size(), getPolygonCount(), (int)round(60 * 16.67 / fullDelta), delta);

		SDL_SetWindowTitle(window, title);

		SDL_Event event;

		if (SDL_PollEvent(&event) && event.type == SDL_QUIT) {
			break;
		}
	}
}