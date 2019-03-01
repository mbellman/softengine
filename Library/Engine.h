#pragma once

#include <cmath>
#include <map>
#include <vector>

#include <SDL.h>
#include <SDL_ttf.h>

#include <Graphics/Rasterizer.h>
#include <Graphics/RasterFilter.h>
#include <Graphics/TriangleBuffer.h>
#include <Graphics/Illuminator.h>
#include <UI/UI.h>
#include <System/Flags.h>
#include <System/DebugStats.h>
#include <System/Objects.h>
#include <System/Geometry.h>
#include <System/Level.h>
#include <System/Math.h>
#include <System/Positionable.h>
#include <Sound/AudioEngine.h>

/**
 * FrustumCuller
 * -------------
 *
 * Aggregates counter variables for polygon vertices culled on
 * any sides of the viewing frustum, and provides a simple
 * mechanism for determining whether the represented polygon
 * should be culled from rendering.
 */
struct FrustumCuller {
	int top = 0;
	int bottom = 0;
	int left = 0;
	int right = 0;
	int near = 0;
	int far = 0;

	/**
	 * In order to correctly determine whether a polygon is culled,
	 * we need to ensure that all of its vertices are culled on a
	 * particular side of the viewing frustum. Since checks are
	 * per-vertex, a polygon with some vertices culled on one side
	 * and some on another might actually spread across the screen,
	 * meaning that it should nevertheless be rendered. This becomes
	 * significant when viewing polygons from extremely close up.
	 */
	bool isCulled() {
		return left == 3 || right == 3 || top == 3 || bottom == 3 || near == 3 || far == 3;
	}
};

/**
 * Engine
 * ------
 */
class Engine {
public:
	Engine(int width, int height, Uint32 flags = 0);
	~Engine();

	void addUIObject(UIObject* uiObject);
	void setActiveLevel(Level* level);
	void run();

private:
	constexpr static float NEAR_Z = 30.0f;
	constexpr static int SERIAL_ILLUMINATION_STATIC_TRIANGLE_LIMIT = 2500;

	DebugStats debugStats;
	std::map<const char*, UIText*> debugStatsTextMap;

	SDL_Window* window;
	SDL_Renderer* renderer;
	Rasterizer* rasterizer;
	RasterFilter* rasterFilter;
	TriangleBuffer* triangleBuffer;
	Illuminator* illuminator;
	AudioEngine* audioEngine;
	UI* ui;
	Level* activeLevel = NULL;
	Camera camera;
	Uint32 flags = 0;
	int width;
	int HALF_W;
	int height;
	int HALF_H;

	enum RenderStep {
		ILLUMINATION,
		SCANLINE_RASTERIZATION
	};

	struct RenderWorkerManager {
		Engine* engine;
		int sectionId;
		RenderStep step;
		bool isWorking = false;
	};

	RenderWorkerManager* renderWorkerManagers;
	std::vector<SDL_Thread*> renderWorkerThreads;
	SDL_Thread* renderThread = NULL;
	bool isRendering = false;
	bool isDone = false;
	int frame = 0;

	static int handleRenderWorkerThread(void* data);
	static int handleRenderThread(void* data);
	void awaitRenderStep(RenderStep renderStep);
	void clearActiveLevel();
	void createRenderThreads();
	void precomputeStaticLightColorIntensities();

	void projectAndQueueTriangle(
		const Vertex3d (&vertexes)[3],
		const Vec3 (&unitVecs)[3],
		const Vec3 (&worldVecs)[3],
		const Polygon* sourcePolygon,
		float normalizedDotProduct,
		float scale,
		bool isSynthetic
	);

	void update(int dt);
	void updateScene_MultiThreaded();
	void updateScene_SingleThreaded();
	void updateScene_Wireframe();
	void updateScreenProjection();
	void updateSounds();

	/* ----- */

	TTF_Font* debugFont = NULL;

	void addDebugStats();
	void addDebugStat(const char* key);
	void updateDebugStat(const char* key, const char* label, int debugValue);
	void updateDebugStats();
};
