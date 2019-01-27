#define _CRT_SECURE_NO_WARNINGS

#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <Objects.h>
#include <Rasterizer.h>
#include <Helpers.h>
#include <Engine.h>

Engine::Engine(int width, int height, Uint32 flags) {
	SDL_Init(SDL_INIT_EVERYTHING);

	window = SDL_CreateWindow(
		"HEY ZACK",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		width, height,
		SDL_WINDOW_SHOWN
	);

	renderer = SDL_CreateRenderer(window, -1, flags & DEBUG_DRAWTIME ? 0 : SDL_RENDERER_PRESENTVSYNC);
	rasterizer = new Rasterizer(renderer, width, height);

	this->width = width;
	this->height = height;
	this->flags = flags;
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

	if (ms > 0) {
		while ((SDL_GetTicks() - startTime) < ms) {
			SDL_Delay(1);
		}
	}
}

void Engine::draw() {
	RotationMatrix rotationMatrix = RotationMatrix::calculate(camera.rotation);
	int fovScalar = 500 * (360 / camera.fov);

	for (int o = 0; o < objects.size(); o++) {
		Object* object = objects.at(o);
		Vec3 relativeObjectPosition = object->position - camera.position;

		object->forEachPolygon([=](const Polygon& polygon) {
			Triangle triangle;
			bool isInView = false;

			for (int i = 0; i < 3; i++) {
				Vec3 vertex = rotationMatrix * (relativeObjectPosition + polygon.vertices[i]->vector);
				Vec3 unitVertex = vertex.unit();
				float distortionCorrectedZ = unitVertex.z * std::abs(std::cos(unitVertex.x));
				int x = (int)(fovScalar * unitVertex.x / (1 + unitVertex.z) + width / 2);
				int y = (int)(fovScalar * -unitVertex.y / (1 + distortionCorrectedZ) + height / 2);

				if (!isInView && unitVertex.z > 0) {
					isInView = true;
				}

				triangle.createVertex(i, { x, y }, (int)vertex.z, polygon.vertices[i]->color);
			}

			if (isInView) {
				if (flags & SHOW_WIREFRAME) {
					rasterizer->setColor(255, 255, 255);

					rasterizer->triangle(
						triangle.vertices[0].coordinate.x, triangle.vertices[0].coordinate.y,
						triangle.vertices[1].coordinate.x, triangle.vertices[1].coordinate.y,
						triangle.vertices[2].coordinate.x, triangle.vertices[2].coordinate.y
					);
				} else {
					rasterizer->triangle(triangle);
				}
			}
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

void Engine::move(float x, float y, float z) {
	camera.position.x += x;
	camera.position.y += y;
	camera.position.z += z;
}

void Engine::run() {
	int lastStartTime;
	bool hasQuit = false;

	while (!hasQuit) {
		lastStartTime = SDL_GetTicks();

		move(velocity.x, velocity.y, velocity.z);
		draw();

		int delta = SDL_GetTicks() - lastStartTime;

		if (flags & DEBUG_DRAWTIME) {
			if (delta < 17) {
				delay(17 - delta);
			} else {
				printf("[DRAW TIME WARNING] ");
			}
			
			printf("Unlocked delta: %d\n", delta);
		}

		int fullDelta = SDL_GetTicks() - lastStartTime;
		char title[100];

		sprintf(title, "Objects: %d, Polygons: %d, FPS: %dfps, Unlocked delta: %dms", objects.size(), getPolygonCount(), (int)round(60 * 17 / fullDelta), delta);

		SDL_SetWindowTitle(window, title);

		SDL_Event event;
		float speed = 5;

		if (SDL_PollEvent(&event)) {
			switch (event.type) {
				// Throwaway code, I promise
				case SDL_KEYDOWN:
					if (event.key.keysym.sym == SDLK_w) {
						velocity.z = speed;
					}
					else if (event.key.keysym.sym == SDLK_s) {
						velocity.z = -speed;
					}
					else if (event.key.keysym.sym == SDLK_a) {
						velocity.x = -speed;
					}
					else if (event.key.keysym.sym == SDLK_d) {
						velocity.x = speed;
					}
					break;
				case SDL_KEYUP:
					if (event.key.keysym.sym == SDLK_w) {
						velocity.z = 0;
					}
					else if (event.key.keysym.sym == SDLK_s) {
						velocity.z = 0;
					}
					else if (event.key.keysym.sym == SDLK_a) {
						velocity.x = 0;
					}
					else if (event.key.keysym.sym == SDLK_d) {
						velocity.x = 0;
					}
					break;
				case SDL_QUIT:
					hasQuit = true;
					break;
			}
		}
	}
}
