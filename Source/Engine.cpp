#define _CRT_SECURE_NO_WARNINGS

#include <SDL.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <algorithm>
#include <Objects.h>
#include <Rasterizer.h>
#include <Helpers.h>
#include <Engine.h>
#include <Quaternion.h>

RotationMatrix Camera::getRotationMatrix() {
	Quaternion q1 = Quaternion::fromAxisAngle(pitch, 1, 0, 0);
	Quaternion q2 = Quaternion::fromAxisAngle(yaw, 0, 1, 0);

	return (q1 * q2).toRotationMatrix();
}

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
	rasterizer = new Rasterizer(renderer, width, height, flags & FLAT_SHADING ? false : true);

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
	RotationMatrix rotationMatrix = camera.getRotationMatrix();
	int fovScalar = 500 * (360 / camera.fov);

	for (int o = 0; o < objects.size(); o++) {
		Object* object = objects.at(o);
		Vec3 relativeObjectPosition = object->position - camera.position;

		object->forEachPolygon([=](const Polygon& polygon) {
			Vec3 polygonPosition = relativeObjectPosition + polygon.vertices[0]->vector;
			bool isFacingCamera = Vec3::dotProduct(polygon.normal, polygonPosition) < 0;

			if (!isFacingCamera) {
				return;
			}

			Triangle triangle;
			bool isInView = false;

			for (int i = 0; i < 3; i++) {
				Vec3 vertex = rotationMatrix * (relativeObjectPosition + polygon.vertices[i]->vector);
				Vec3 unitVertex = vertex.unit();
				float distortionCorrectedZ = unitVertex.z * std::abs(std::cos(unitVertex.x));
				int x = (int)(fovScalar * unitVertex.x / (1 + unitVertex.z) + width / 2);
				int y = (int)(fovScalar * -unitVertex.y / (1 + distortionCorrectedZ) + height / 2);
				int depth = (int)vertex.z;

				if (!isInView && depth > 0) {
					isInView = true;
				}

				triangle.createVertex(i, x, y, depth, polygon.vertices[i]->color);
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

void Engine::handleEvent(const SDL_Event& event) {
	switch (event.type) {
		case SDL_KEYDOWN:
			handleKeyDown(event.key.keysym.sym);
			break;
		case SDL_KEYUP:
			handleKeyUp(event.key.keysym.sym);
			break;
		case SDL_MOUSEMOTION:
			handleMouseMotionEvent(event.motion);
			break;
	}
}

void Engine::handleKeyDown(const SDL_Keycode& code) {
	switch (code) {
		case SDLK_w: movement.z = 1; break;
		case SDLK_s: movement.z = -1; break;
		case SDLK_a: movement.x = -1; break;
		case SDLK_d: movement.x = 1; break;
	}
}

void Engine::handleKeyUp(const SDL_Keycode& code) {
	switch (code) {
		case SDLK_w: movement.z = 0; break;
		case SDLK_s: movement.z = 0; break;
		case SDLK_a: movement.x = 0; break;
		case SDLK_d: movement.x = 0; break;
		case SDLK_ESCAPE:
		case SDLK_SPACE:
			SDL_SetRelativeMouseMode(SDL_FALSE);
			break;
	}
}

void Engine::handleMouseMotionEvent(const SDL_MouseMotionEvent& event) {
	bool isRelativeMouseMode = SDL_GetRelativeMouseMode();
	int xDelta = isRelativeMouseMode ? -event.xrel : 0;
	int yDelta = isRelativeMouseMode ? -event.yrel : 0;
	float deltaFactor = 1.0f / 500;

	camera.pitch = clamp(camera.pitch + (float)yDelta * deltaFactor, -Camera::MAX_PITCH, Camera::MAX_PITCH);
	camera.yaw += (float)xDelta * deltaFactor;
}

void Engine::run() {
	int lastStartTime;
	bool isRunning = true;

	SDL_SetRelativeMouseMode(SDL_TRUE);

	while (isRunning) {
		lastStartTime = SDL_GetTicks();

		updateMovement();
		draw();

		int delta = SDL_GetTicks() - lastStartTime;

		if (flags & DEBUG_DRAWTIME) {
			if (delta < 17) {
				delay(17 - delta);
			} else {
				std::cout << "[DRAW TIME WARNING] ";
			}

			std::cout << "Unlocked delta: " << delta << "\n";
		}

		int fullDelta = SDL_GetTicks() - lastStartTime;
		char title[100];

		sprintf(title, "Objects: %d, Polygons: %d, FPS: %dfps, Unlocked delta: %dms", objects.size(), getPolygonCount(), (int)round(60 * 17 / fullDelta), delta);

		SDL_SetWindowTitle(window, title);

		SDL_Event event;
		float speed = 5;

		while (SDL_PollEvent(&event)) {
			handleEvent(event);

			if (event.type == SDL_QUIT) {
				isRunning = false;
				break;
			}
		}
	}
}

void Engine::updateMovement() {
	float sy = std::sin(camera.yaw);
	float cy = std::cos(camera.yaw);

	float xDelta = movement.x * cy - movement.z * sy;
	float zDelta = movement.z * cy + movement.x * sy;

	camera.position.x += MOVEMENT_SPEED * xDelta;
	camera.position.z += MOVEMENT_SPEED * zDelta;
}
