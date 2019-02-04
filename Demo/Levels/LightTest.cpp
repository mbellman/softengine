#include <SDL.h>
#include <Levels/LightTest.h>
#include <Objects.h>
#include <Loaders/ObjLoader.h>

/**
 * LightTest
 * ---------
 */
void LightTest::load() {
	ObjLoader icoObj("./DemoAssets/da-vinci.obj");

	Mesh* mesh = new Mesh(50, 20, 50);
	mesh->position = { -500, 0, 0 };

	Model* icosahedron = new Model(icoObj);
	icosahedron->position = { 0, 100, 500 };
	icosahedron->scale(100);

	add(mesh);
	add(icosahedron);

	Mesh* leftWall = new Mesh(50, 10, 50);
	Mesh* rightWall = new Mesh(50, 10, 50);

	leftWall->position = { -500, 500, 0 };
	leftWall->rotate({ 0, 0, 1.5 * M_PI });

	rightWall->position = { 500, 0, 0 };
	rightWall->rotate({ 0, 0, M_PI / 2 });

	Light* light = new Light();
	light->position = { 0, 200, 500 };
	light->color = { 255, 220, 50 };
	light->spread = 1000;

	Light* light2 = new Light();
	light2->position = { 200, 200, 1500 };
	light2->color = { 255, 0, 0 };
	light2->spread = 700;

	Light* light3 = new Light();
	light3->position = { -200, 150, 2200 };
	light3->color = { 0, 0, 255 };
	light3->spread = 600;
	light3->power = 1;

	add(leftWall);
	add(rightWall);
	add(light);
	add(light2);
	add(light3);

	setBackgroundColor(0, 0, 0);
}
