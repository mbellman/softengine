#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <cmath>
#include <map>
#include <algorithm>
#include <string>

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>

#include <Engine.h>
#include <Helpers.h>
#include <Constants.h>
#include <System/Quaternion.h>
#include <System/Objects.h>
#include <System/Geometry.h>
#include <System/Camera.h>
#include <System/Scene.h>
#include <System/Flags.h>
#include <UI/UIObjects.h>
#include <UI/Alert.h>
#include <Graphics/Rasterizer.h>
#include <Graphics/RasterFilter.h>
#include <Graphics/TextureBuffer.h>
#include <Graphics/TriangleBuffer.h>
#include <Graphics/Illuminator.h>
#include <System/DebugStats.h>

using namespace std;

/**
 * Engine
 * ------
 */
Engine::Engine(int width, int height, const char* title, const char* iconPath, const char* debugFontPath, int flags) {
	SDL_Init(SDL_INIT_EVERYTHING);
	TTF_Init();
	IMG_Init(IMG_INIT_PNG);

	Uint32 windowFlags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;

	if (flags & DISABLE_WINDOW_RESIZE) {
		windowFlags &= ~SDL_WINDOW_RESIZABLE;
	}

	window = SDL_CreateWindow(
		title,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		width, height,
		windowFlags
	);

	if (iconPath != NULL) {
		setWindowIcon(iconPath);
	}

	if (debugFontPath != NULL) {
		debugFont = TTF_OpenFont(debugFontPath, 15);

		if (!debugFont) {
			char errorMessage[60];

			sprintf(errorMessage, "Unable to load font: %s", debugFontPath);
			Alert::error(ALERT_ASSET_ERROR, errorMessage);
			exit(0);
		}
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
	triangleBuffer = new TriangleBuffer();
	illuminator = new Illuminator();
	audioEngine = new AudioEngine();
	ui = new UI(renderer);
	commandLine = new CommandLine();

	windowArea.width = width;
	windowArea.height = height;

	this->flags = flags;

	resizeRasterRegion();

	if (~flags & DISABLE_MULTITHREADING) {
		createRenderThreads();
	}
}

Engine::~Engine() {
	isDone = true;

	for (int i = 0; i < renderWorkerThreads.size(); i++) {
		SDL_WaitThread(renderWorkerThreads.at(i), NULL);
	}

	if (renderThread != NULL) {
		SDL_WaitThread(renderThread, NULL);
	}

	delete triangleBuffer;
	delete illuminator;
	delete rasterFilter;
	delete ui;
	delete rasterizer;
	delete audioEngine;
	delete commandLine;

	TTF_CloseFont(debugFont);
	TTF_Quit();

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void Engine::awaitRenderStep(RenderStep renderStep) {
	for (int i = 0; i < renderWorkerThreads.size(); i++) {
		RenderWorkerManager* manager = &renderWorkerManagers[i];

		manager->step = renderStep;
		manager->isWorking = true;
	}

	for (int i = 0; i < renderWorkerThreads.size(); i++) {
		while (renderWorkerManagers[i].isWorking) {
			SDL_Delay(1);
		}
	}
}

void Engine::createRenderThreads() {
	// Adhering to a 1-active-thread-per-core limit, we can allot
	// as many render worker threads as cores are available after
	// the 1) main thread and 2) primary rendering threads are
	// discounted.
	int totalRenderWorkerThreads = SDL_GetCPUCount() - 2;

	if (totalRenderWorkerThreads < 1) {
		// If we don't even have enough cores available for 1 render
		// worker thread, forgo multithreading entirely.
		return;
	}

	renderWorkerManagers = new RenderWorkerManager[totalRenderWorkerThreads];

	// Create render worker threads
	for (int i = 0; i < totalRenderWorkerThreads; i++) {
		RenderWorkerManager* manager = &renderWorkerManagers[i];

		manager->engine = this;
		manager->sectionId = i;

		SDL_Thread* thread = SDL_CreateThread(Engine::handleRenderWorkerThread, NULL, manager);
		renderWorkerThreads.push_back(thread);
	}

	// Create main render thread
	renderThread = SDL_CreateThread(Engine::handleRenderThread, NULL, this);
}

int Engine::getFlags() {
	return flags;
}

int Engine::getWindowHeight() {
	return windowArea.height;
}

int Engine::getWindowWidth() {
	return windowArea.width;
}

/**
 * Runner for a render worker thread. Depending on the step in
 * the rendering pipeline, render workers either handle illumination
 * or scanline rasterization in parallel with one another, each
 * managing an isolated set of triangles (for illumination) or
 * scanlines (for rasterization) to avoid race conditions.
 */
int Engine::handleRenderWorkerThread(void* data) {
	SDL_SetThreadPriority(SDL_THREAD_PRIORITY_TIME_CRITICAL);

	RenderWorkerManager* manager = (RenderWorkerManager*)data;
	Engine* engine = manager->engine;
	TriangleBuffer* triangleBuffer = engine->triangleBuffer;
	Illuminator* illuminator = engine->illuminator;

	while (1) {
		Rasterizer* currentRasterizer = engine->rasterizer;

		if (engine->isDone) {
			break;
		} else if (manager->isWorking) {
			int totalRenderWorkerThreads = engine->renderWorkerThreads.size();

			switch (manager->step) {
				case RenderStep::ILLUMINATION: {
					const auto& bufferedTriangles = triangleBuffer->getBufferedTriangles();

					for (int i = 0; i < bufferedTriangles.size(); i++) {
						// Each render worker gets to illuminate every Nth triangle,
						// where N is the number of available workers.
						if (i % totalRenderWorkerThreads == manager->sectionId) {
							Triangle* triangle = bufferedTriangles.at(i);

							illuminator->illuminateTriangle(triangle);
						}
					}

					break;
				}
				case RenderStep::SCANLINE_RASTERIZATION: {
					for (int i = 0; i < currentRasterizer->getTotalBufferedScanlines(); i++) {
						const Scanline* scanline = currentRasterizer->getScanline(i);

						// Each render worker gets to rasterize scanlines on every
						// Nth screen row, where N is the number of available workers.
						if (scanline->y % totalRenderWorkerThreads == manager->sectionId) {
							currentRasterizer->triangleScanline(scanline);
						}
					}

					break;
				}
				default:
					break;
			}

			manager->isWorking = false;
		}

		SDL_Delay(1);
	}

	return 0;
}

/**
 * Runner for the 'primary' render thread, which is distinct from the
 * main thread. The render thread is in charge of signaling render
 * worker threads to perform parallel illumination, followed by
 * scanline rasterization. The main thread in turn is responsible
 * for signaling to the render thread that a new frame has begun,
 * and previous-frame rendering can occur in parallel with next-frame
 * screen projection and raster filtering.
 */
int Engine::handleRenderThread(void* data) {
	SDL_SetThreadPriority(SDL_THREAD_PRIORITY_TIME_CRITICAL);

	Engine* engine = (Engine*)data;
	TriangleBuffer* triangleBuffer = engine->triangleBuffer;
	Illuminator* illuminator = engine->illuminator;
	DebugStats& debugStats = engine->debugStats;

	while (1) {
		Rasterizer* currentRasterizer = engine->rasterizer;

		if (engine->isDone) {
			break;
		} else if (engine->isRendering) {
			debugStats.trackIlluminationTime();

			if (triangleBuffer->getTotalNonStaticTriangles() > SERIAL_ILLUMINATION_NONSTATIC_TRIANGLE_LIMIT) {
				engine->awaitRenderStep(RenderStep::ILLUMINATION);
			} else {
				for (auto* triangle : triangleBuffer->getBufferedTriangles()) {
					illuminator->illuminateTriangle(triangle);
				}
			}

			debugStats.logIlluminationTime();
			debugStats.trackDrawTime();

			// Triangles cannot be dispatched to the rasterizer in parallel,
			// since triangles are buffered in approximate order from closest
			// to furthest, helping to mitigate overdraw.
			for (auto* triangle : triangleBuffer->getBufferedTriangles()) {
				currentRasterizer->dispatchTriangle(*triangle);
			}

			// Once all triangles are dispatched to the rasterizer and their
			// scanlines queued up, we can parallelize the actual scanlines.
			engine->awaitRenderStep(RenderStep::SCANLINE_RASTERIZATION);
			debugStats.logDrawTime();

			engine->isRendering = false;
		}

		SDL_Delay(1);
	}

	return 0;
}

bool Engine::hasStopped() {
	return isStopped;
}

void Engine::initialize() {
	isStopped = false;

	if (debugFont != NULL && (flags & DEBUG_STATS)) {
		addDebugStats();
	}

	if (debugFont != NULL && (flags & DEBUG_COMMAND_LINE)) {
		addCommandLineText();
		hideCommandLine();
	}

	SDL_SetRelativeMouseMode(SDL_TRUE);
}

void Engine::lockProportionalRasterRegion(int xp, int yp, int wp, int hp) {
	rasterLockRegion.x = xp;
	rasterLockRegion.y = yp;
	rasterLockRegion.width = wp;
	rasterLockRegion.height = hp;

	resizeRasterRegion();
}

/**
 * Precomputes and caches static ambient or static light source
 * color intensities on Polygons belonging to static Objects,
 * avoiding the need to recalculate these values during runtime.
 */
void Engine::precomputeStaticLightColorIntensities() {
	auto precomputeObjectLight = [=](Object* object) {
		for (auto* polygon : object->getPolygons()) {
			illuminator->illuminateStaticPolygon(polygon);
		}
	};

	for (auto* object : activeScene->getObjects()) {
		if (!object->isStatic || !object->hasLighting) {
			continue;
		}

		precomputeObjectLight(object);

		for (auto* lod : object->getLODs()) {
			precomputeObjectLight(lod);
		}
	}
}

/**
 * Projects and queues a triangle into the raster filter using
 * a set of three vertices, three unit vectors, and three world
 * vectors representing to the original location of the source
 * polygon, as well as additional values to aid with lighting
 * and scaling calculations. For efficiency we forward the
 * arguments from drawScene(), which has already computed them.
 */
void Engine::projectAndQueueTriangle(
	const Vertex3d (&vertexes)[3],
	const Vec3 (&unitVecs)[3],
	const Vec3 (&worldVecs)[3],
	const Polygon* sourcePolygon,
	float normalizedDotProduct,
	float scale,
	bool isSynthetic
) {
	float objectFresnelFactor = sourcePolygon->sourceObject->fresnelFactor;
	Triangle* triangle = triangleBuffer->requestTriangle();

	triangle->sourcePolygon = const_cast<Polygon*>(sourcePolygon);
	triangle->isSynthetic = isSynthetic;
	triangle->fresnelFactor = objectFresnelFactor > 0 ? cosf(normalizedDotProduct * (M_PI / 2.0f)) * objectFresnelFactor : 0.0f;

	for (int i = 0; i < 3; i++) {
		const Vertex3d& vertex3d = vertexes[i];
		const Vec3& vector = vertex3d.vector;
		const Vec3& unit = unitVecs[i];
		float inverseDepth = 1.0f / vector.z;

		Vertex2d* vertex = &triangle->vertices[i];

		vertex->coordinate.x = (int)(scale * unit.x / unit.z + halfRasterArea.width);
		vertex->coordinate.y = (int)(scale * -unit.y / unit.z + halfRasterArea.height);
		vertex->z = vector.z;
		vertex->inverseDepth = inverseDepth;
		vertex->perspectiveUV = vertex3d.uv * inverseDepth;
		vertex->color = vertex3d.color;
		vertex->worldVector = worldVecs[i];
		vertex->normal = vertex3d.normal;
	}

	rasterFilter->addTriangle(triangle);
}

void Engine::resizeRasterRegion() {
	rasterRegion.x = windowArea.width * (rasterLockRegion.x / 100.0f);
	rasterRegion.y = windowArea.height * (rasterLockRegion.y / 100.0f);
	rasterRegion.width = (int)std::round(windowArea.width * (rasterLockRegion.width / 100.0f));
	rasterRegion.height = (int)std::round(windowArea.height * (rasterLockRegion.height / 100.0f));

	bool hasPixelFilter = flags & PIXEL_FILTER;
	int rasterWidth = hasPixelFilter ? rasterRegion.width / 2 : rasterRegion.width;
	int rasterHeight = hasPixelFilter ? rasterRegion.height / 2 : rasterRegion.height;

	halfRasterArea.width = (int)(rasterWidth / 2);
	halfRasterArea.height = (int)(rasterHeight / 2);

	if (rasterizer != NULL) {
		delete rasterizer;
	}

	if (rasterFilter != NULL) {
		delete rasterFilter;
	}

	rasterizer = new Rasterizer(renderer, rasterWidth, rasterHeight);
	rasterFilter = new RasterFilter(rasterWidth, rasterHeight);

	rasterizer->setOffset({ rasterRegion.x, rasterRegion.y });
}

void Engine::setActiveScene(Scene* scene) {
	activeScene = scene;

	triangleBuffer->resetAll();
	illuminator->setActiveScene(scene);
	commandLine->setActiveScene(scene);
	audioEngine->mute();

	if (!scene->hasInitialized) {
		scene->provideUI(new UI(renderer));
		scene->load();
		scene->onStart();

		scene->hasInitialized = true;
	}

	updateSounds();
	precomputeStaticLightColorIntensities();

	audioEngine->unmute();
}

void Engine::setWindowIcon(const char* icon) {
	SDL_Surface* image = IMG_Load(icon);

	if (!image) {
		char errorMessage[60];

		sprintf(errorMessage, "Unable to load icon: %s", icon);
		Alert::error(ALERT_ASSET_ERROR, errorMessage);
		exit(0);
	}

	SDL_SetWindowIcon(window, image);
	SDL_FreeSurface(image);
}

void Engine::stop() {
	isStopped = true;
}

void Engine::update(int dt) {
	if (hasStopped()) {
		return;
	}

	int startTime = SDL_GetTicks();
	const Settings& settings = activeScene->settings;

	debugStats.trackFrameTime();

	// Update view
	SDL_RenderClear(renderer);

	rasterizer->setBackgroundColor(settings.backgroundColor);
	rasterizer->setVisibility(settings.visibility);
	rasterizer->clear();

	updateSounds();

	if (flags & SHOW_WIREFRAME) {
		updateScene_Wireframe();
	} else if (renderWorkerThreads.size() > 0) {
		updateScene_MultiThreaded();
	} else {
		updateScene_SingleThreaded();
	}

	activeScene->ui->update(dt);
	ui->update(dt);

	// Handle inputs
	SDL_Event event;

	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT) {
			stop();

			return;
		} else if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED) {
			windowArea.width = event.window.data1;
			windowArea.height = event.window.data2;

			resizeRasterRegion();
		} else if ((flags & DEBUG_COMMAND_LINE) && event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_c) {
			showCommandLine();
		} else if (commandLine->isOpen()) {
			handleCommandLineInput(event);
		} else {
			activeScene->inputManager->handleEvent(event);
		}
	}

	// Advance game logic
	debugStats.trackUpdateTime();
	activeScene->update(dt);
	debugStats.logUpdateTime();

	// Frame lock checks, debug stat updates, render to screen
	if (flags & FPS_30) {
		while ((SDL_GetTicks() - startTime) < 33) {
			SDL_Delay(0);
		}
	}

	debugStats.logFrameTime();

	if (debugFont != NULL && (flags & DEBUG_STATS)) {
		updateDebugStats();
	}

	if (debugFont != NULL && (flags & DEBUG_COMMAND_LINE)) {
		updateCommandLineText();
	}

	SDL_RenderPresent(renderer);

	triangleBuffer->reset();
	debugStats.reset();

	frame++;
}

void Engine::toggleFlag(Flags flag) {
	if (flags & flag) {
		flags &= ~flag;
	} else {
		flags |= flag;
	}

	switch (flag) {
		case PIXEL_FILTER:
			resizeRasterRegion();
			break;
		case DISABLE_WINDOW_RESIZE:
			SDL_SetWindowResizable(window, (flags & DISABLE_WINDOW_RESIZE) ? SDL_FALSE : SDL_TRUE);
			break;
	}
}

/**
 * Updates the game scene using parallelization mechanisms.
 */
void Engine::updateScene_MultiThreaded() {
	// In mulithreaded mode, we wait a full frame before the
	// first render pass. The scene needs to be projected and
	// buffered once; after this, next-frame projection/raster
	// filtering can occur while previous-frame rendering occurs
	// in parallel.
	if (frame > 0) {
		// Signal the main render thread to kick off the
		// rendering pipeline while we perform screen
		// projection/raster filtering here
		isRendering = true;
	}

	debugStats.trackScreenProjectionTime();

	updateScreenProjection();

	debugStats.logScreenProjectionTime();
	debugStats.trackHiddenSurfaceRemovalTime();

	Triangle* triangle;

	while ((triangle = rasterFilter->next()) != NULL) {
		triangleBuffer->bufferTriangle(triangle);
	}

	debugStats.logHiddenSurfaceRemovalTime();

	if (frame > 0) {
		while (isRendering) {
			SDL_Delay(1);
		}

		rasterizer->render(renderer, (flags & PIXEL_FILTER) ? 2 : 1);
	}
}

/**
 * Updates the game scene in a serial fashion when multithreading is
 * either disabled or unavailable due to limited available CPU cores.
 */
void Engine::updateScene_SingleThreaded() {
	debugStats.trackScreenProjectionTime();

	updateScreenProjection();

	debugStats.logScreenProjectionTime();
	debugStats.trackHiddenSurfaceRemovalTime();

	Triangle* triangle;

	while ((triangle = rasterFilter->next()) != NULL) {
		triangleBuffer->bufferTriangle(triangle);
	}

	debugStats.logHiddenSurfaceRemovalTime();
	debugStats.trackIlluminationTime();

	for (auto* triangle : triangleBuffer->getBufferedTriangles()) {
		illuminator->illuminateTriangle(triangle);
	}

	debugStats.logIlluminationTime();
	debugStats.trackDrawTime();

	for (auto* triangle : triangleBuffer->getBufferedTriangles()) {
		rasterizer->dispatchTriangle(*triangle);
	}

	for (int i = 0; i < rasterizer->getTotalBufferedScanlines(); i++) {
		rasterizer->triangleScanline(rasterizer->getScanline(i));
	}

	rasterizer->render(renderer, (flags & PIXEL_FILTER) ? 2 : 1);
	debugStats.logDrawTime();
}

/**
 * Updates the scene with wireframes only; all illumination,
 * coloring, and texturing steps are skipped.
 */
void Engine::updateScene_Wireframe() {
	debugStats.trackScreenProjectionTime();

	updateScreenProjection();

	debugStats.logScreenProjectionTime();
	debugStats.trackHiddenSurfaceRemovalTime();

	Triangle* triangle;

	while ((triangle = rasterFilter->next()) != NULL) {
		triangleBuffer->bufferTriangle(triangle);
	}

	debugStats.logHiddenSurfaceRemovalTime();
	debugStats.trackIlluminationTime();
	debugStats.logIlluminationTime();
	debugStats.trackDrawTime();

	for (auto* triangle : triangleBuffer->getBufferedTriangles()) {
		rasterizer->triangle(
			triangle->vertices[0].coordinate.x, triangle->vertices[0].coordinate.y,
			triangle->vertices[1].coordinate.x, triangle->vertices[1].coordinate.y,
			triangle->vertices[2].coordinate.x, triangle->vertices[2].coordinate.y
		);
	}

	rasterizer->render(renderer, (flags & PIXEL_FILTER) ? 2 : 1);
	debugStats.logDrawTime();
}

void Engine::updateScreenProjection() {
	const Camera& camera = activeScene->getCamera();
	float projectionScale = (float)max(halfRasterArea.width, halfRasterArea.height) * (180.0f / camera.fov);
	float fovAngleRange = sinf(DEG_TO_RAD * camera.fov / 2.0f);
	RotationMatrix cameraRotationMatrix = camera.getRotationMatrix();

	// Allocate reusable vertex/vector objects up front to
	// be overwritten/projected with each subsequent polygon
	Vertex3d t_verts[3];
	Vec3 u_vecs[3];
	Vec3 w_vecs[3];

	for (const auto* object : activeScene->getObjects()) {
		Vec3 relativeObjectPosition = object->position - camera.position;
		const Object* lodObject = object->hasLODs() ? object->getLOD(relativeObjectPosition.magnitude()) : object;

		if (!activeScene->isInCurrentOccupiedSector(object->sectorId)) {
			continue;
		}

		debugStats.countPolygons(lodObject->getPolygonCount());
		debugStats.countVertices(lodObject->getVertexCount());

		if (lodObject->texture != NULL) {
			lodObject->texture->confirmTexture(renderer, TextureMode::SOFTWARE);
		}

		for (const auto* polygon : lodObject->getPolygons()) {
			Vec3 relativePolygonPosition = relativeObjectPosition + polygon->vertices[0]->vector;
			float normalizedDotProduct = Vec3::dotProduct(polygon->normal, relativePolygonPosition.unit());

			// As hack to fix polygons viewed at or near glancing angles
			// being rendered as holes in meshes, we allow polygons through
			// even when they are very marginally back-facing.
			bool isFacingCamera = normalizedDotProduct < 0.05f;

			if (!isFacingCamera) {
				continue;
			}

			FrustumCuller frustumCuller;

			// Build our vertex/unit + world vector lists while we perform
			// view frustum clipping checks on the polygon
			for (int i = 0; i < 3; i++) {
				t_verts[i] = *polygon->vertices[i];
				t_verts[i].vector = cameraRotationMatrix * (relativeObjectPosition + polygon->vertices[i]->vector);
				u_vecs[i] = t_verts[i].vector.unit();
				w_vecs[i] = object->position + polygon->vertices[i]->vector;

				if (t_verts[i].vector.z < NEAR_PLANE_DISTANCE) {
					frustumCuller.near++;
				} else if (t_verts[i].vector.z > activeScene->settings.visibility) {
					frustumCuller.far++;
				}

				if (u_vecs[i].x < -fovAngleRange) {
					frustumCuller.left++;
				} else if (u_vecs[i].x > fovAngleRange) {
					frustumCuller.right++;
				}

				if (u_vecs[i].y < -fovAngleRange) {
					frustumCuller.bottom++;
				} else if (u_vecs[i].y > fovAngleRange) {
					frustumCuller.top++;
				}
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
						(t_verts[0].vector.z - object->nearClippingDistance) / (t_verts[0].vector.z - t_verts[1].vector.z),
						(t_verts[0].vector.z - object->nearClippingDistance) / (t_verts[0].vector.z - t_verts[2].vector.z)
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
						polygon, normalizedDotProduct, projectionScale, true
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
					float v2Delta = (t_verts[1].vector.z - object->nearClippingDistance) / (t_verts[1].vector.z - t_verts[2].vector.z);
					float v3Delta = (t_verts[0].vector.z - object->nearClippingDistance) / (t_verts[0].vector.z - t_verts[2].vector.z);

					// Define new vertices + unit/world vectors for the quad
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
						polygon, normalizedDotProduct, projectionScale, true
					);

					projectAndQueueTriangle(
						{ quadVerts[0], quadVerts[2], quadVerts[3] },
						{ u_quadVecs[0], u_quadVecs[2], u_quadVecs[3] },
						{ w_quadVecs[0], w_quadVecs[2], w_quadVecs[3] },
						polygon, normalizedDotProduct, projectionScale, true
					);
				}
			} else {
				// Project a regular, unclipped triangle
				projectAndQueueTriangle(
					t_verts, u_vecs, w_vecs,
					polygon, normalizedDotProduct, projectionScale, false
				);
			}
		}
	}
}

void Engine::updateSounds() {
	const Camera& camera = activeScene->getCamera();
	RotationMatrix cameraRotationMatrix = camera.getRotationMatrix();

	for (auto* sound : activeScene->getSounds()) {
		Vec3 relativeSoundPosition = cameraRotationMatrix * (sound->position - camera.position);

		sound->setApparentPosition(relativeSoundPosition * 0.1f);
	}
}

// --------- DEBUGGING --------- //

void Engine::addDebugStats() {
	addDebugStat("screenProjectionTime");
	addDebugStat("hsrTime");
	addDebugStat("illuminationTime");
	addDebugStat("drawTime");
	addDebugStat("updateTime");
	addDebugStat("frameTime");
	addDebugStat("fps");
	addDebugStat("totalVertices");
	addDebugStat("totalTriangles");
	addDebugStat("totalTrianglesProjected");
	addDebugStat("totalTrianglesDrawn");
	addDebugStat("totalScanlines");
}

void Engine::addCommandLineText() {
	UIText* text = new UIText();

	text->setValue("> ");
	text->setFont(debugFont);
	text->position = { 10, windowArea.height + 30 };

	ui->add("commandLineText", text);
}

void Engine::updateDebugStats() {
	updateDebugStat("screenProjectionTime", "Screen projection time", debugStats.getScreenProjectionTime());
	updateDebugStat("hsrTime", "Hidden surface removal time", debugStats.getHiddenSurfaceRemovalTime());
	updateDebugStat("illuminationTime", "Illumination time", debugStats.getIlluminationTime());
	updateDebugStat("drawTime", "Draw time", debugStats.getDrawTime());
	updateDebugStat("updateTime", "Update time", debugStats.getUpdateTime());
	updateDebugStat("frameTime", "Frame time", debugStats.getFrameTime());
	updateDebugStat("fps", "FPS", debugStats.getFPS());
	updateDebugStat("totalVertices", "Vertices", debugStats.getTotalVertices(activeScene->getObjects()));
	updateDebugStat("totalTriangles", "Triangles", debugStats.getTotalPolygons(activeScene->getObjects()));
	updateDebugStat("totalTrianglesProjected", "Triangles projected", triangleBuffer->getTotalRequestedTriangles());
	updateDebugStat("totalTrianglesDrawn", "Triangles drawn", triangleBuffer->getBufferedTriangles().size());
	updateDebugStat("totalScanlines", "Scanlines", rasterizer->getTotalBufferedScanlines());
}

void Engine::addDebugStat(const char* key) {
	UIText* text = new UIText();

	text->position = { 10, 10 + totalDebugStats * 25 };
	text->setFont(debugFont);

	ui->add(key, text);

	totalDebugStats++;
}

void Engine::updateDebugStat(const char* key, const char* label, int value) {
	char statString[50];

	sprintf(statString, "%s: %d", label, value);

	UIText* text = (UIText*)ui->get(key);

	text->setValue(statString);
}

void Engine::showCommandLine() {
	commandLine->open();
	ui->get("commandLineText")->tweenTo({ 10, windowArea.height - 30 }, 500, Ease::quadOut);
}

void Engine::hideCommandLine() {
	ui->get("commandLineText")->tweenTo({ 10, windowArea.height + 30 }, 500, Ease::quadOut);
	commandLine->close();
}

void Engine::handleCommandLineInput(const SDL_Event& event) {
	if (event.type == SDL_KEYUP) {
		switch (event.key.keysym.sym) {
			case SDLK_BACKSPACE:
				commandLine->backspace();
				break;
			case SDLK_RETURN:
				commandLine->executeCurrentCommand();
				precomputeStaticLightColorIntensities();
				hideCommandLine();
				break;
			case SDLK_ESCAPE:
				hideCommandLine();
				break;
			default:
				break;
		}
	} else if (event.type == SDL_TEXTINPUT) {
		commandLine->queueCharacter(event.text.text[0]);
	}
}

void Engine::updateCommandLineText() {
	UIText* commandLineText = (UIText*)ui->get("commandLineText");

	commandLineText->setValue(("> " + commandLine->getCurrentCommand()).c_str());
}
