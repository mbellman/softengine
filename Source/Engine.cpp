#define _CRT_SECURE_NO_WARNINGS

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <cmath>
#include <map>
#include <algorithm>

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>

#include <Engine.h>
#include <Helpers.h>
#include <System/Quaternion.h>
#include <System/Objects.h>
#include <System/Geometry.h>
#include <UI/UIObjects.h>
#include <Graphics/Rasterizer.h>
#include <Graphics/RasterFilter.h>
#include <Graphics/TextureBuffer.h>
#include <System/DebugStats.h>

using namespace std;

RotationMatrix Camera::getRotationMatrix() {
	Quaternion q1 = Quaternion::fromAxisAngle(pitch, 1, 0, 0);
	Quaternion q2 = Quaternion::fromAxisAngle(yaw, 0, 1, 0);

	return (q1 * q2).toRotationMatrix();
}

Engine::Engine(int width, int height, Uint32 flags) {
	SDL_Init(SDL_INIT_EVERYTHING);
	TTF_Init();
	IMG_Init(IMG_INIT_PNG);

	window = SDL_CreateWindow(
		"Engine Test",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		width, height,
		SDL_WINDOW_SHOWN
	);

	bool hasPixelFilter = flags & PIXEL_FILTER;
	int rasterWidth = hasPixelFilter ? width / 2 : width;
	int rasterHeight = hasPixelFilter ? height / 2 : height;

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
	rasterizer = new Rasterizer(renderer, rasterWidth, rasterHeight, flags);
	rasterFilter = new RasterFilter(rasterWidth, rasterHeight);
	ui = new UI();
	audioEngine = new AudioEngine();
	triangles = new Triangle[250000];

	debugFont = TTF_OpenFont("./DemoAssets/FreeMono.ttf", 15);

	this->width = width;
	this->height = height;
	this->flags = flags;

	HALF_W = (int)(width / (hasPixelFilter ? 4 : 2));
	HALF_H = (int)(height / (hasPixelFilter ? 4 : 2));

	createIlluminationThreads();
}

Engine::~Engine() {
	isDone = true;

	for (int i = 0; i < illuminationThreads.size(); i++) {
		SDL_WaitThread(illuminationThreads.at(i), NULL);
	}

	triangleRasterBuffer.clear();

	delete rasterFilter;
	delete ui;
	delete rasterizer;
	delete audioEngine;
	delete[] triangles;

	if (flags & DEBUG_STATS) {
		for (auto& [key, uiText] : debugStatsTextMap) {
			delete uiText;
		}

		debugStatsTextMap.clear();
	}

	TTF_CloseFont(debugFont);
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

void Engine::createIlluminationThreads() {
	int totalThreads = SDL_GetCPUCount();

	if (totalThreads < 2) {
		return;
	}

	illuminationThreadManagers = new IlluminationThreadManager[totalThreads];

	for (int i = 0; i < totalThreads; i++) {
		IlluminationThreadManager* manager = &illuminationThreadManagers[i];

		manager->engine = this;
		manager->section = i;

		SDL_Thread* thread = SDL_CreateThread(manageIlluminationThread, NULL, manager);
		illuminationThreads.push_back(thread);
	}
}

void Engine::drawTriangle(Triangle* triangle) {
	if (flags & SHOW_WIREFRAME) {
		rasterizer->setDrawColor(255, 255, 255);

		rasterizer->triangle(
			triangle->vertices[0].coordinate.x, triangle->vertices[0].coordinate.y,
			triangle->vertices[1].coordinate.x, triangle->vertices[1].coordinate.y,
			triangle->vertices[2].coordinate.x, triangle->vertices[2].coordinate.y
		);
	} else {
		rasterizer->triangle(*triangle);
	}

	debugStats.countDrawnTriangle();
}

/**
 * Returns the 3-component color intensity of a given triangle
 * vertex, using the triangle itself and a reference to one of
 * its vertices.
 */
Vec3 Engine::getTriangleVertexColorIntensity(const Triangle* triangle, int vertexIndex) {
	const Vertex2d& vertex = triangle->vertices[vertexIndex];
	const Settings& settings = activeLevel->getSettings();
	std::map<int, Vec3>& vertexLightCache = triangle->sourcePolygon->vertexLightCache[vertexIndex];
	Vec3 colorIntensity = { 1.0f, 1.0f, 1.0f };

	colorIntensity *= settings.brightness;

	if (settings.brightness == 0) {
		return colorIntensity;
	}

	// Ambient light is a special distance-invariant light
	// source which affects all geometry in the level
	if (settings.ambientLightFactor > 0) {
		const auto& cachedAmbientLight = vertexLightCache.find(Engine::AMBIENT_LIGHT_ID);
		bool isCacheableLightSource = settings.hasStaticAmbientLight && triangle->sourceObject->isStatic && !triangle->isSynthetic;

		if (isCacheableLightSource && cachedAmbientLight != vertexLightCache.end()) {
			const Vec3& cachedAmbientLightIntensity = cachedAmbientLight->second;

			colorIntensity *= cachedAmbientLightIntensity;
		} else {
			float dot = Vec3::dotProduct(triangle->sourcePolygon->normal, settings.ambientLightVector.unit());

			if (dot < 0) {
				float incidence = cosf((1 + dot) * M_PI / 2);
				float intensity = incidence * settings.ambientLightFactor;
				const Vec3& colorRatios = settings.ambientLightColor.ratios();

				Vec3 ambientLightColorIntensity = {
					(1.0f + (intensity * colorRatios.x) / settings.brightness),
					(1.0f + (intensity * colorRatios.y) / settings.brightness),
					(1.0f + (intensity * colorRatios.z) / settings.brightness)
				};

				colorIntensity *= ambientLightColorIntensity;

				if (isCacheableLightSource) {
					vertexLightCache.emplace(Engine::AMBIENT_LIGHT_ID, ambientLightColorIntensity);
				}
			}
		}
	}

	// Regular light sources must be within range of a vertex
	// to affect its color intensity
	for (const auto* light : activeLevel->getLights()) {
		bool isCacheableLightSource = light->isStatic && triangle->sourceObject->isStatic && !triangle->isSynthetic;

		if (isCacheableLightSource) {
			const auto& cachedLight = vertexLightCache.find(light->getId());

			if (cachedLight != vertexLightCache.end()) {
				const Vec3& cachedLightIntensity = cachedLight->second;

				colorIntensity *= cachedLightIntensity;

				continue;
			}
		}

		if (
			light->isDisabled ||
			light->power == 0 ||
			abs(light->position.x - vertex.worldVector.x) > light->range ||
			abs(light->position.y - vertex.worldVector.y) > light->range ||
			abs(light->position.z - vertex.worldVector.z) > light->range
		) {
			continue;
		}

		Vec3 lightVector = vertex.worldVector - light->position;
		float lightDistance = lightVector.magnitude();

		if (lightDistance < light->range) {
			float dot = Vec3::dotProduct(triangle->sourcePolygon->normal, lightVector.unit());

			if (dot < 0) {
				float incidence = cosf((1 + dot) * M_PI / 2);
				float illuminance = pow(1.0f - lightDistance / light->range, 2);
				float intensity = light->power * incidence * illuminance;
				const Vec3& colorRatios = light->getColorRatios();

				Vec3 lightColorIntensity = {
					(1.0f + (intensity * colorRatios.x) / settings.brightness),
					(1.0f + (intensity * colorRatios.y) / settings.brightness),
					(1.0f + (intensity * colorRatios.z) / settings.brightness)
				};

				colorIntensity *= lightColorIntensity;

				if (isCacheableLightSource) {
					vertexLightCache.emplace(light->getId(), lightColorIntensity);
				}
			}
		}
	}

	return colorIntensity;
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

	camera.pitch = std::clamp(camera.pitch + (float)yDelta * deltaFactor, -Camera::MAX_PITCH, Camera::MAX_PITCH);
	camera.yaw += (float)xDelta * deltaFactor;
}

void Engine::illuminateColorTriangle(Triangle* triangle) {
	const Settings& settings = activeLevel->getSettings();

	for (int i = 0; i < 3; i++) {
		Vertex2d* vertex = &triangle->vertices[i];
		const Vec3 colorIntensity = getTriangleVertexColorIntensity(triangle, i);

		vertex->color.R *= colorIntensity.x;
		vertex->color.G *= colorIntensity.y;
		vertex->color.B *= colorIntensity.z;
		vertex->color.clamp();

		float visibilityRatio = FAST_MIN(vertex->z / settings.visibility, 1.0f);

		vertex->color = Color::lerp(vertex->color, settings.backgroundColor, visibilityRatio);
	}
}

void Engine::illuminateScene() {
	if (illuminationThreads.size() == 0) {
		for (auto* triangle : triangleRasterBuffer) {
			if (triangle->sourceObject->texture != NULL) {
				illuminateTextureTriangle(triangle);
			} else {
				illuminateColorTriangle(triangle);
			}
		}
	} else {
		for (int i = 0; i < illuminationThreads.size(); i++) {
			illuminationThreadManagers[i].isDone = false;
		}

		for (int i = 0; i < illuminationThreads.size(); i++) {
			while (!illuminationThreadManagers[i].isDone) {
				SDL_Delay(1);
			}
		}
	}
}

void Engine::illuminateTextureTriangle(Triangle* triangle) {
	const Settings& settings = activeLevel->getSettings();

	for (int i = 0; i < 3; i++) {
		Vertex2d* vertex = &triangle->vertices[i];
		vertex->textureIntensity = getTriangleVertexColorIntensity(triangle, i);
	}
}

int Engine::manageIlluminationThread(void* data) {
	IlluminationThreadManager* manager = (IlluminationThreadManager*)data;
	Engine* engine = manager->engine;

	while(1) {
		if (engine->isDone) {
			break;
		} else if (!manager->isDone) {
			int totalThreads = engine->illuminationThreads.size();

			for (int i = 0; i < engine->triangleRasterBuffer.size(); i++) {
				if (i % totalThreads == manager->section) {
					Triangle* triangle = engine->triangleRasterBuffer.at(i);

					if (triangle->sourceObject->texture != NULL) {
						engine->illuminateTextureTriangle(triangle);
					} else {
						engine->illuminateColorTriangle(triangle);
					}
				}
			}

			manager->isDone = true;
		}

		SDL_Delay(1);
	}

	return 0;
}

/**
 * Projects and queues a triangle into the raster filter using
 * a set of three vertices, three unit vectors, and three world
 * vectors representing to the original location of the source
 * polygon, as well as the triangle's source object and polygon for
 * contextual information. For efficiency we forward the arguments
 * from drawScene(), which has already computed them.
 */
void Engine::projectAndQueueTriangle(
	const Vertex3d (&vertexes)[3],
	const Vec3 (&unitVecs)[3],
	const Vec3 (&worldVecs)[3],
	const Object* sourceObject,
	const Polygon* sourcePolygon,
	float scale,
	bool isSynthetic
) {
	Triangle* triangle = &triangles[totalProjectedTriangles++];

	triangle->sourcePolygon = const_cast<Polygon*>(sourcePolygon);
	triangle->sourceObject = sourceObject;
	triangle->isSynthetic = isSynthetic;

	for (int i = 0; i < 3; i++) {
		const Vertex3d& vertex3d = vertexes[i];
		const Vec3& vector = vertex3d.vector;
		const Vec3& unit = unitVecs[i];

		Vertex2d* vertex = &triangle->vertices[i];

		vertex->coordinate.x = (int)(scale * unit.x / unit.z + HALF_W);
		vertex->coordinate.y = (int)(scale * -unit.y / unit.z + HALF_H);
		vertex->z = vector.z;
		vertex->inverseDepth = 1.0f / vector.z;
		vertex->perspectiveUV = vertex3d.uv / vector.z;
		vertex->color = vertex3d.color;
		vertex->worldVector = worldVecs[i];
	}

	rasterFilter->addTriangle(triangle);
	debugStats.countProjectedTriangle();
}

void Engine::run() {
	if (activeLevel == NULL) {
		return;
	}

	if (flags & DEBUG_STATS) {
		addDebugStats();
	}

	int lastStartTime;
	bool hasStarted = false;

	SDL_SetRelativeMouseMode(SDL_TRUE);

	audioEngine->mute();
	activeLevel->load();

	while (!activeLevel->hasQuit()) {
		lastStartTime = SDL_GetTicks();

		update();

		if (!hasStarted) {
			activeLevel->onStart();
			audioEngine->unmute();

			hasStarted = true;
		}

		int delta = SDL_GetTicks() - lastStartTime;

		activeLevel->update(delta);
		activeLevel->onUpdate(delta, SDL_GetTicks());

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

	activeLevel = level;
}

void Engine::update() {
	const Settings& settings = activeLevel->getSettings();

	rasterizer->setBackgroundColor(settings.backgroundColor);
	rasterizer->setVisibility(settings.visibility);
	rasterizer->clear();

	debugStats.resetCounters();
	debugStats.trackFrameTime();

	updateMovement();
	updateScene();
	updateSounds();
	ui->draw();

	debugStats.logFrameTime();

	if (flags & DEBUG_STATS) {
		updateDebugStats();
	}

	SDL_RenderPresent(renderer);

	totalProjectedTriangles = 0;
	triangleRasterBuffer.clear();
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

void Engine::updateScene() {
	debugStats.trackScreenProjectionTime();

	bool hasPixelFilter = flags & PIXEL_FILTER;
	float projectionScale = (float)max(HALF_W, HALF_H) * (180.0f / camera.fov);
	float fovAngleRange = sinf(((float)camera.fov / 2) * M_PI / 180);
	RotationMatrix cameraRotationMatrix = camera.getRotationMatrix();

	// Allocate reusable vertex/vector objects up front to
	// be overwritten/projected with each subsequent polygon
	Vertex3d t_verts[3];
	Vec3 u_vecs[3];
	Vec3 w_vecs[3];

	for (const auto* object : activeLevel->getObjects()) {
		Vec3 relativeObjectPosition = object->position - camera.position;

		if (object->texture != NULL) {
			object->texture->confirmTexture(renderer, TextureMode::SOFTWARE, ~flags & DISABLE_MIPMAPPING);
		}

		for (const auto& polygon : object->getPolygons()) {
			Vec3 polygonPosition = relativeObjectPosition + polygon.vertices[0]->vector;
			bool isFacingCamera = Vec3::dotProduct(polygon.normal, polygonPosition) < 0;

			if (!isFacingCamera) {
				continue;
			}

			FrustumCuller frustumCuller;

			// Build our vertex/unit + world vector lists while we perform
			// view frustum clipping checks on the polygon
			for (int i = 0; i < 3; i++) {
				t_verts[i] = *polygon.vertices[i];
				t_verts[i].vector = cameraRotationMatrix * (relativeObjectPosition + polygon.vertices[i]->vector);
				u_vecs[i] = t_verts[i].vector.unit();
				w_vecs[i] = object->position + polygon.vertices[i]->vector;

				if (t_verts[i].vector.z < Engine::NEAR_Z)
					frustumCuller.near++;
				else if (t_verts[i].vector.z > activeLevel->getSettings().visibility)
					frustumCuller.far++;

				if (u_vecs[i].x < -fovAngleRange)
					frustumCuller.left++;
				else if (u_vecs[i].x > fovAngleRange)
					frustumCuller.right++;

				if (u_vecs[i].y < -fovAngleRange)
					frustumCuller.bottom++;
				else if (u_vecs[i].y > fovAngleRange)
					frustumCuller.top++;
			}

			if (frustumCuller.isCulled()) {
				continue;
			}

			if (frustumCuller.near > 0) {
				// If any vertices are behind the near plane, we have to
				// clip them against it. This is necessary to prevent
				// erroneous screen projections at coordinates <= 0.

				// Sort vertices by descending z-order so we can determine
				// where to interpolate the clipped vertices
				if (t_verts[0].vector.z < t_verts[1].vector.z) {
					swap(t_verts[0], t_verts[1]);
					swap(u_vecs[0], u_vecs[1]);
					swap(w_vecs[0], w_vecs[1]);
				}

				if (t_verts[1].vector.z < t_verts[2].vector.z) {
					swap(t_verts[1], t_verts[2]);
					swap(u_vecs[1], u_vecs[2]);
					swap(w_vecs[1], w_vecs[2]);
				}

				if (t_verts[0].vector.z < t_verts[1].vector.z) {
					swap(t_verts[0], t_verts[1]);
					swap(u_vecs[0], u_vecs[1]);
					swap(w_vecs[0], w_vecs[1]);
				}

				if (frustumCuller.near == 2) {
					// When two of the polygon's vertices are behind the near
					// plane, it can be clipped into a smaller polygon at the
					// plane boundary.

					// Determine interpolation deltas for each new vertex
					// (the first need not be interpolated at all)
					float deltas[3] = {
						0.0f,
						(t_verts[0].vector.z - Engine::NEAR_Z) / (t_verts[0].vector.z - t_verts[1].vector.z),
						(t_verts[0].vector.z - Engine::NEAR_Z) / (t_verts[0].vector.z - t_verts[2].vector.z)
					};

					// Generate new vertices and unit/world vectors for the clipped polygon
					for (int i = 1; i < 3; i++) {
						t_verts[i] = Vertex3d::lerp(t_verts[0], t_verts[i], deltas[i]);
						u_vecs[i] = t_verts[i].vector.unit();
						w_vecs[i] = Vec3::lerp(w_vecs[0], w_vecs[i], deltas[i]);
					}

					// Project the clipped polygon
					projectAndQueueTriangle(
						t_verts, u_vecs, w_vecs,
						object, &polygon, projectionScale, true
					);
				} else if (frustumCuller.near == 1) {
					// If only one of the polygon's vertices is behind the
					// near plane, we need to clip it into a quad, which then
					// needs to be clipped into two polygons. The first and
					// second vertices can be preserved, whereas the latter
					// two will have to be interpolated between the second and
					// third, and first and third original vertices.
					Vertex3d quadVerts[4];
					Vec3 u_quadVecs[4];
					Vec3 w_quadVecs[4];

					// Determine interpolation deltas for third and fourth vertices
					float v2Delta = (t_verts[1].vector.z - Engine::NEAR_Z) / (t_verts[1].vector.z - t_verts[2].vector.z);
					float v3Delta = (t_verts[0].vector.z - Engine::NEAR_Z) / (t_verts[0].vector.z - t_verts[2].vector.z);

					// Define new vertices/unit + world vectors for the quad
					quadVerts[0] = t_verts[0];
					quadVerts[1] = t_verts[1];
					quadVerts[2] = Vertex3d::lerp(t_verts[1], t_verts[2], v2Delta);
					quadVerts[3] = Vertex3d::lerp(t_verts[0], t_verts[2], v3Delta);

					u_quadVecs[0] = quadVerts[0].vector.unit();
					u_quadVecs[1] = quadVerts[1].vector.unit();
					u_quadVecs[2] = quadVerts[2].vector.unit();
					u_quadVecs[3] = quadVerts[3].vector.unit();

					w_quadVecs[0] = w_vecs[0];
					w_quadVecs[1] = w_vecs[1];
					w_quadVecs[2] = Vec3::lerp(w_vecs[1], w_vecs[2], v2Delta);
					w_quadVecs[3] = Vec3::lerp(w_vecs[0], w_vecs[2], v3Delta);

					// Project the quad's two polygons individually
					projectAndQueueTriangle(
						{ quadVerts[0], quadVerts[1], quadVerts[2] },
						{ u_quadVecs[0], u_quadVecs[1], u_quadVecs[2] },
						{ w_quadVecs[0], w_quadVecs[1], w_quadVecs[2] },
						object, &polygon, projectionScale, true
					);

					projectAndQueueTriangle(
						{ quadVerts[0], quadVerts[2], quadVerts[3] },
						{ u_quadVecs[0], u_quadVecs[2], u_quadVecs[3] },
						{ w_quadVecs[0], w_quadVecs[2], w_quadVecs[3] },
						object, &polygon, projectionScale, true
					);
				}
			} else {
				// Project a regular, unclipped triangle
				projectAndQueueTriangle(
					t_verts, u_vecs, w_vecs,
					object, &polygon, projectionScale, false
				);
			}
		}
	}

	debugStats.logScreenProjectionTime();

	Triangle* triangle;

	while ((triangle = rasterFilter->next()) != NULL) {
		triangleRasterBuffer.push_back(triangle);
	}

	debugStats.trackIlluminationTime();

	// Illumination step
	illuminateScene();

	// for (auto* triangle : triangleRasterBuffer) {
	// 	if (triangle->sourceObject->texture != NULL) {
	// 		illuminateTextureTriangle(triangle);
	// 	} else {
	// 		illuminateColorTriangle(triangle);
	// 	}
	// }

	debugStats.logIlluminationTime();
	debugStats.trackDrawTime();

	for (auto* triangle : triangleRasterBuffer) {
		drawTriangle(triangle);
	}

	rasterizer->render(renderer, hasPixelFilter ? 2 : 1);
	debugStats.logDrawTime();
}

void Engine::updateSounds() {
	RotationMatrix cameraRotationMatrix = camera.getRotationMatrix();

	for (auto* sound : activeLevel->getSounds()) {
		Vec3 relativeSoundPosition = cameraRotationMatrix * (sound->position - camera.position);

		sound->setApparentPosition(relativeSoundPosition * 0.1f);
	}
}

// --------- DEBUG STATS --------- //

void Engine::addDebugStats() {
	addDebugStat("screenProjectionTime");
	addDebugStat("illuminationTime");
	addDebugStat("drawTime");
	addDebugStat("frameTime");
	addDebugStat("fps");
	addDebugStat("totalVertices");
	addDebugStat("totalTriangles");
	addDebugStat("totalTrianglesProjected");
	addDebugStat("totalTrianglesDrawn");
	addDebugStat("totalScanlines");
}

void Engine::updateDebugStats() {
	updateDebugStat("screenProjectionTime", "Screen projection time", debugStats.getScreenProjectionTime());
	updateDebugStat("illuminationTime", "Illumination time", debugStats.getIlluminationTime());
	updateDebugStat("drawTime", "Draw time", debugStats.getDrawTime());
	updateDebugStat("frameTime", "Frame time", debugStats.getFrameTime());
	updateDebugStat("fps", "FPS", debugStats.getFPS());
	updateDebugStat("totalVertices", "Vertices", debugStats.getTotalVertices(activeLevel->getObjects()));
	updateDebugStat("totalTriangles", "Triangles", debugStats.getTotalPolygons(activeLevel->getObjects()));
	updateDebugStat("totalTrianglesProjected", "Triangles projected", totalProjectedTriangles);
	updateDebugStat("totalTrianglesDrawn", "Triangles drawn", debugStats.getTotalDrawnTriangles());
	updateDebugStat("totalScanlines", "Scanlines", rasterizer->getTotalScanlines());
}

void Engine::addDebugStat(const char* key) {
	UIText* text = new UIText();

	text->setPosition(10, 10 + (debugStatsTextMap.size() * 25));
	text->setRenderer(renderer);
	text->setFont(debugFont);

	debugStatsTextMap.emplace(key, text);
}

void Engine::updateDebugStat(const char* key, const char* label, int value) {
	char statString[50];

	sprintf(statString, "%s: %d", label, value);

	UIText* text = debugStatsTextMap.at(key);

	text->setValue(statString);
	text->draw();
}
