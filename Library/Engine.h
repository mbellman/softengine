#pragma once

#include <SDL.h>
#include <math.h>
#include <vector>
#include <Objects.h>
#include <Level.h>
#include <Types.h>
#include <UI/UI.h>
#include <Graphics/Rasterizer.h>
#include <Graphics/RasterQueue.h>

/**
 * Flags
 * -----
 */
enum Flags : Uint32 {
	DEBUG_DRAWTIME = 1 << 0,
	SHOW_WIREFRAME = 1 << 1,
	FLAT_SHADING = 1 << 2,
	PIXEL_FILTER = 1 << 3,
	REMOVE_OCCLUDED_SURFACES = 1 << 4
};

/**
 * Camera
 * ------
 */
struct Camera {
	constexpr static float MAX_PITCH = 89 * M_PI / 180;
	Vec3 position = { 0, 100, 0 };
	float pitch = 0.0f;
	float yaw = 0.0f;
	int fov = 90;

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
	constexpr static int MOVEMENT_SPEED = 5;
	constexpr static int ZONE_RANGE = 250;

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
	int height;

	void clearActiveLevel();
	void delay(int ms);
	void drawScene();
	void drawTriangle(Triangle& triangle);
	int getPolygonCount();
	void handleEvent(const SDL_Event& event);
	void handleKeyDown(const SDL_Keycode& code);
	void handleKeyUp(const SDL_Keycode& code);
	void handleMouseMotionEvent(const SDL_MouseMotionEvent& event);
	void illuminateTriangle(Triangle& triangle);
	void update();
	void updateMovement();
};
