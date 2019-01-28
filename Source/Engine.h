#pragma once

#include <SDL.h>
#include <math.h>
#include <vector>
#include <Rasterizer.h>
#include <Objects.h>

enum Flags: Uint32 {
	DEBUG_DRAWTIME = 1 << 0,
	SHOW_WIREFRAME = 1 << 1
};

struct Camera {
	Vec3 position = { 0, 100, 0 };
	Vec3 rotation = { 0, 0, 0 };
	int fov = 90;
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
		void draw();
		void run();
	private:
		SDL_Window* window;
		SDL_Renderer* renderer;
		std::vector<Object*> objects;
		Rasterizer* rasterizer;
		Camera camera;
		Coordinate lastMouseCoordinate;
		Vec3 velocity;
		Movement movement;
		Uint32 flags = 0;
		constexpr static int MOVEMENT_SPEED = 5;
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