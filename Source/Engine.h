#pragma once

#include <SDL.h>
#include <math.h>
#include <vector>
#include <Rasterizer.h>
#include <Objects.h>

struct Camera {
	Vec3 position = { 0, 100, 0 };
	float pitch = 0.0f;
	float yaw = 0.0f;
};

class Engine {
	public:
		Engine(int width, int height);
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
		int width;
		int height;
		int getPolygonCount();
		void delay(int ms);
};