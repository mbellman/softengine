#pragma once

#include <SDL.h>
#include <math.h>
#include <vector>
#include <Rasterizer.h>
#include <Objects.h>

struct Camera {
	Vec3 position;
	float pitch = 0.0f;
	float yaw = 0.0f;
};

class Engine {
	public:
		Engine();
		~Engine();
		void addObject(Object* object);
		void draw();
		void run(int width, int height);
	private:
		SDL_Window* window;
		SDL_Renderer* renderer;
		std::vector<Object*> objects;
		Rasterizer* rasterizer;
		Camera camera;
		int getPolygonCount();
		void delay(int ms);
};