#define _CRT_SECURE_NO_WARNINGS

#include <SDL.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <algorithm>
#include <Objects.h>
#include <Helpers.h>
#include <Engine.h>
#include <Quaternion.h>
#include <UI/UIObjects.h>
#include <Graphics/Rasterizer.h>
#include <Graphics/RasterQueue.h>

RotationMatrix Camera::getRotationMatrix() {
	Quaternion q1 = Quaternion::fromAxisAngle(pitch, 1, 0, 0);
	Quaternion q2 = Quaternion::fromAxisAngle(yaw, 0, 1, 0);

	return (q1 * q2).toRotationMatrix();
}

Engine::Engine(int width, int height, Uint32 flags) {
	SDL_Init(SDL_INIT_EVERYTHING);
	TTF_Init();

	window = SDL_CreateWindow(
		"HEY ZACK",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		width, height,
		SDL_WINDOW_SHOWN
	);

	int rasterWidth = flags & PIXEL_FILTER ? width / 2 : width;
	int rasterHeight = flags & PIXEL_FILTER ? height / 2 : height;

	renderer = SDL_CreateRenderer(window, -1, flags & DEBUG_DRAWTIME ? 0 : SDL_RENDERER_PRESENTVSYNC);
	rasterizer = new Rasterizer(renderer, rasterWidth, rasterHeight, ~flags & FLAT_SHADING);
	rasterQueue = new RasterQueue(rasterWidth, rasterHeight);
	ui = new UI();

	this->width = width;
	this->height = height;
	this->flags = flags;
}

Engine::~Engine() {
	delete rasterizer;
	delete ui;

	TTF_Quit();

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void Engine::addUIObject(UIObject* uiObject) {
	uiObject->setRenderer(renderer);
	ui->addObject(uiObject);
}

void Engine::clearActiveLevel() {
	if (activeLevel != NULL) {
		delete activeLevel;
	}

	activeLevel = NULL;
}

void Engine::delay(int ms) {
	int startTime = SDL_GetTicks();

	if (ms > 0) {
		while ((SDL_GetTicks() - startTime) < ms) {
			SDL_Delay(1);
		}
	}
}

void Engine::drawTriangle(Triangle& triangle) {
	if (flags & SHOW_WIREFRAME) {
		rasterizer->setColor(255, 255, 255);

		rasterizer->triangle(
			triangle.vertices[0].coordinate.x, triangle.vertices[0].coordinate.y,
			triangle.vertices[1].coordinate.x, triangle.vertices[1].coordinate.y,
			triangle.vertices[2].coordinate.x, triangle.vertices[2].coordinate.y
		);
	} else {
		const Polygon* parentPolygon = triangle.polygon;
		const Object* parentObject = parentPolygon->object;

		// Before rasterizing the triangle, one more pass is necessary to
		// determine the aggregate effect of area lights on each vertex color.
		for (int i = 0; i < 3; i++) {
			Color aggregateLightColor = { 0, 0, 0 };
			Vertex2d* screenVertex = &triangle.vertices[i];
			Vec3 worldVertex = (parentObject->position + parentPolygon->vertices[i]->vector);

			for (auto* light : activeLevel->getLights()) {
				if (
					abs(light->position.x - worldVertex.x) > light->spread ||
					abs(light->position.y - worldVertex.y) > light->spread ||
					abs(light->position.z - worldVertex.z) > light->spread
				) {
					// If the light source is further away along any axis than
					// its range, we can automatically ignore it.
					continue;
				}

				Vec3 lightVector = worldVertex - light->position;
				float lightDistance = lightVector.magnitude();

				if (lightDistance < light->spread) {
					float dot = Vec3::dotProduct(parentPolygon->normal, lightVector.unit());

					if (dot < 0) {
						float incidence = cosf((1 + dot) * M_PI / 2);
						float intensity = pow(1.0f - lightDistance / light->spread, 2);

						aggregateLightColor += light->color * (light->power * incidence * intensity);

						// For each incident light source, reverse any potential
						// ambient light color diminishment on the vertex in
						// proportion to the light's intensity at this point
						screenVertex->color *= (1 + (intensity / ambientLight));
					}
				}
			}

			float distanceRatio = std::clamp((float)screenVertex->depth / drawDistance, 0.0f, 1.0f);

			screenVertex->color = lerp(screenVertex->color + aggregateLightColor, activeLevel->getBackgroundColor(), distanceRatio);
		}

		rasterizer->triangle(triangle);
	}
}

void Engine::drawScene() {
	bool hasPixelFilter = flags & PIXEL_FILTER;
	bool shouldRemoveOccludedSurfaces = flags & REMOVE_OCCLUDED_SURFACES;

	float fovAngleRange = sinf((((float)camera.fov / 2)) * M_PI / 180);
	int fovScalar = (hasPixelFilter ? 250 : 500) * (360 / camera.fov);
	int midpointX = width / (hasPixelFilter ? 4 : 2);
	int midpointY = height / (hasPixelFilter ? 4 : 2);

	RotationMatrix rotationMatrix = camera.getRotationMatrix();

	for (auto* object : activeLevel->getObjects()) {
		Vec3 relativeObjectPosition = object->position - camera.position;

		for (auto& polygon : object->getPolygons()) {
			// Check #1: Ensure that the polygon is facing the camera
			Vec3 polygonPosition = relativeObjectPosition + polygon.vertices[0]->vector;
			bool isFacingCamera = Vec3::dotProduct(polygon.normal, polygonPosition) < 0;

			if (!isFacingCamera) {
				continue;
			}

			// Check #2: Ensure that the polygon is within the camera view frustum
			Vec3 localSpaceVertices[3];
			Vec3 unitVertices[3];
			bool isWithinViewFrustum = false;

			for (int i = 0; i < 3; i++) {
				localSpaceVertices[i] = rotationMatrix * (relativeObjectPosition + polygon.vertices[i]->vector);
				unitVertices[i] = localSpaceVertices[i].unit();

				if (
					!isWithinViewFrustum &&
					abs(unitVertices[i].x) < fovAngleRange &&
					abs(unitVertices[i].y) < fovAngleRange &&
					localSpaceVertices[i].z > 0 && localSpaceVertices[i].z < drawDistance
				) {
					isWithinViewFrustum = true;
				}
			}

			if (!isWithinViewFrustum) {
				continue;
			}

			// Once a Polygon has passed the back-face and view frustum
			// culling checks, we can create its Triangle projection.
			Triangle triangle;

			triangle.polygon = &polygon;

			for (int i = 0; i < 3; i++) {
				Vec3& localSpaceVertex = localSpaceVertices[i];
				Vec3& unitVertex = unitVertices[i];
				float distortionCorrectedZ = unitVertex.z * cosf(unitVertex.x);
				int x = (int)(fovScalar * unitVertex.x / (1 + unitVertex.z) + midpointX);
				int y = (int)(fovScalar * -unitVertex.y / (1 + distortionCorrectedZ) + midpointY);
				int depth = (int)(localSpaceVertex.z);
				Color color = polygon.vertices[i]->color * ambientLight;

				triangle.createVertex(i, x, y, depth, color);
			}

			if (shouldRemoveOccludedSurfaces) {
				int zone = (int)(triangle.averageDepth() / Engine::ZONE_RANGE);

				rasterQueue->addTriangle(triangle, zone);
			} else {
				drawTriangle(triangle);
			}
		}
	}

	if (shouldRemoveOccludedSurfaces) {
		Triangle* triangle;

		while ((triangle = rasterQueue->next()) != NULL) {
			drawTriangle(*triangle);
		}
	}

	rasterizer->render(renderer, hasPixelFilter ? 2 : 1);
}

int Engine::getPolygonCount() {
	int total = 0;

	for (auto object : activeLevel->getObjects()) {
		total += object->getPolygonCount();
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
		case SDL_MOUSEBUTTONDOWN:
			if (event.button.button == SDL_BUTTON_LEFT) {
				SDL_SetRelativeMouseMode(SDL_TRUE);
			}
			break;
	}
}

void Engine::handleKeyDown(const SDL_Keycode& code) {
	switch (code) {
		case SDLK_w: movement.z = 1; break;
		case SDLK_s: movement.z = -1; break;
		case SDLK_a: movement.x = -1; break;
		case SDLK_d: movement.x = 1; break;
		case SDLK_LSHIFT: isRunning = true; break;
	}
}

void Engine::handleKeyUp(const SDL_Keycode& code) {
	switch (code) {
		case SDLK_w: movement.z = 0; break;
		case SDLK_s: movement.z = 0; break;
		case SDLK_a: movement.x = 0; break;
		case SDLK_d: movement.x = 0; break;
		case SDLK_LSHIFT: isRunning = false; break;
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
	if (activeLevel == NULL) {
		return;
	}

	int lastStartTime;

	SDL_SetRelativeMouseMode(SDL_TRUE);

	while (!activeLevel->hasQuit()) {
		lastStartTime = SDL_GetTicks();

		update();

		int delta = SDL_GetTicks() - lastStartTime;

		if (flags & DEBUG_DRAWTIME) {
			if (delta < 17) {
				delay(17 - delta);
			} else {
				std::cout << "[DRAW TIME WARNING]: " << delta << "ms\n";
			}
		}

		int fullDelta = SDL_GetTicks() - lastStartTime;

		activeLevel->update(fullDelta, SDL_GetTicks());

		char title[100];
		sprintf(
			title,
			"Polygons: %d, FPS: %dfps, Unlocked delta: %dms",
			getPolygonCount(), (int)round(60 * 17 / fullDelta), delta
		);
		SDL_SetWindowTitle(window, title);

		SDL_Event event;

		while (SDL_PollEvent(&event)) {
			handleEvent(event);

			if (event.type == SDL_QUIT) {
				activeLevel->quit();
				break;
			}
		}
	}

	clearActiveLevel();
}

void Engine::setActiveLevel(Level* level) {
	clearActiveLevel();

	level->load();

	activeLevel = level;
}

void Engine::setDrawDistance(int drawDistance) {
	this->drawDistance = drawDistance;
}

void Engine::update() {
		updateMovement();

		rasterizer->setBackgroundColor(activeLevel->getBackgroundColor());
		rasterizer->clear();

		drawScene();
		ui->draw();

		SDL_RenderPresent(renderer);
}

void Engine::updateMovement() {
	float sy = std::sin(camera.yaw);
	float cy = std::cos(camera.yaw);

	float xDelta = movement.x * cy - movement.z * sy;
	float zDelta = movement.z * cy + movement.x * sy;

	int scalar = (isRunning ? 4 : 1) * MOVEMENT_SPEED;

	camera.position.x += scalar * xDelta;
	camera.position.z += scalar * zDelta;
}
