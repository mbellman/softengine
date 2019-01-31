#pragma once

#include <SDL.h>
#include <math.h>
#include <vector>
#include <Rasterizer.h>
#include <Objects.h>
#include <Types.h>
#include <UI/UI.h>

enum Flags: Uint32 {
	DEBUG_DRAWTIME = 1 << 0,
	SHOW_WIREFRAME = 1 << 1,
	FLAT_SHADING = 1 << 2,
	PIXEL_FILTER = 1 << 3
};

struct Camera {
	Vec3 position = { 0, 100, 0 };
	float pitch = 0.0f;
	float yaw = 0.0f;
	int fov = 90;
	constexpr static float MAX_PITCH = 89 * M_PI / 180;
	RotationMatrix getRotationMatrix();
};

struct Movement {
	int x = 0;
	int z = 0;
};

class Engine {
	public:
		Engine(int width, int height, Uint32 flags = 0);
		~Engine();

		void addObject(Object* object);
		void addUIObject(UIObject* uiObject);
		void draw();
		void run();

	private:
		SDL_Window* window;
		SDL_Renderer* renderer;
		std::vector<Object*> objects;
		Rasterizer* rasterizer;
		UI* ui;

		Camera camera;
		Vec3 velocity;
		Movement movement;
		constexpr static int MOVEMENT_SPEED = 5;
		Uint32 flags = 0;

		int width;
		int height;
		void delay(int ms);
		int getPolygonCount();
		void handleEvent(const SDL_Event& event);
		void handleKeyDown(const SDL_Keycode& code);
		void handleKeyUp(const SDL_Keycode& code);
		void handleMouseMotionEvent(const SDL_MouseMotionEvent& event);
		void updateMovement();
};
