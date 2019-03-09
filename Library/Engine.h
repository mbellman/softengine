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
#include <System/Scene.h>
#include <System/Math.h>
#include <System/Positionable.h>
#include <System/CommandLine.h>
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
	Engine(int width, int height, const char* title, const char* iconPath, const char* debugFontPath, int flags = 0);
	~Engine();

	int getFlags();
	int getWindowHeight();
	int getWindowWidth();
	void lockRasterArea(int x, int y, int w, int h);
	void run();
	void setActiveScene(Scene* scene);
	void stop();
	void toggleFlag(Flags flag);

private:
	SDL_Window* window;
	SDL_Renderer* renderer;
	Rasterizer* rasterizer = NULL;
	RasterFilter* rasterFilter = NULL;
	TriangleBuffer* triangleBuffer;
	Illuminator* illuminator;
	AudioEngine* audioEngine;
	UI* ui;
	CommandLine* commandLine;
	Scene* activeScene = NULL;
	int flags = 0;
	bool isRunning = false;
	bool isRasterAreaLocked = false;
	Coordinate rasterOffset;
	Area windowArea;
	Area rasterArea;
	Area halfRasterArea;

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

	void resizeRasterArea(int w, int h);
	void setWindowIcon(const char* icon);
	void update(int dt);
	void updateScene_MultiThreaded();
	void updateScene_SingleThreaded();
	void updateScene_Wireframe();
	void updateScreenProjection();
	void updateSounds();

	/* --- DEBUGGING -- */

	TTF_Font* debugFont = NULL;
	DebugStats debugStats;
	int totalDebugStats = 0;

	void addDebugStats();
	void addCommandLineText();

	void addDebugStat(const char* key);
	void updateDebugStat(const char* key, const char* label, int debugValue);
	void updateDebugStats();

	void showCommandLine();
	void hideCommandLine();
	void handleCommandLineInput(const SDL_Event& event);
	void updateCommandLineText();
};
