#pragma once

#include <cmath>
#include <map>
#include <vector>

#include <SDL.h>
#include <SDL_ttf.h>

#include <Graphics/Rasterizer.h>
#include <Graphics/RasterQueue.h>
#include <UI/UI.h>
#include <System/Flags.h>
#include <System/DebugStats.h>
#include <System/Objects.h>
#include <System/Level.h>
#include <System/Math.h>

/**
 * Camera
 * ------
 */
struct Camera {
	constexpr static float MAX_PITCH = 89 * M_PI / 180;
	Vec3 position = { 0, 100, 0 };
	float pitch = 0.0f;
	float yaw = 0.0f;
	int fov = 100;

	RotationMatrix getRotationMatrix();
};

/**
 * Movement
 * --------
 */
struct Movement {
	int x = 0;
	int z = 0;
};

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
	constexpr static int MOVEMENT_SPEED = 5;
	constexpr static int AMBIENT_LIGHT_ID = 0;

	DebugStats debugStats;
	std::map<const char*, UIText*> debugStatsTextMap;

	SDL_Window* window;
	SDL_Renderer* renderer;
	Rasterizer* rasterizer;
	RasterQueue* rasterQueue;
	UI* ui;
	Level* activeLevel = NULL;
	Camera camera;
	Vec3 velocity;
	Movement movement;
	bool isRunning = false;
	Uint32 flags = 0;
	int width;
	int HALF_W;
	int height;
	int HALF_H;

	void clearActiveLevel();
	void delay(int ms);
	void drawScene();
	void drawTriangle(Triangle& triangle);
	Vec3 getTriangleVertexColorIntensity(const Triangle& triangle, int vertexIndex);
	void handleEvent(const SDL_Event& event);
	void handleKeyDown(const SDL_Keycode& code);
	void handleKeyUp(const SDL_Keycode& code);
	void handleMouseMotionEvent(const SDL_MouseMotionEvent& event);
	void illuminateColorTriangle(Triangle& triangle);
	void illuminateTextureTriangle(Triangle& triangle);

	void projectAndQueueTriangle(
		const Vertex3d (&vertexes)[3],
		const Vec3 (&unitVecs)[3],
		const Vec3 (&worldVecs)[3],
		const Object* sourceObject,
		const Polygon* sourcePolygon,
		float scale,
		bool isSynthetic
	);

	void update();
	void updateMovement();

	/* ----- */

	TTF_Font* debugFont = NULL;

	void addDebugStats();
	void addDebugStat(const char* key);
	void updateDebugStat(const char* key, const char* label, int debugValue);
	void updateDebugStats();
};
