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
	icosahedron->position = { 0, 200, 500 };
	icosahedron->scale(100);

	add(mesh);
	add(icosahedron);

	Mesh* leftWall = new Mesh(50, 10, 50);
	Mesh* rightWall = new Mesh(50, 10, 50);

	leftWall->position = { -500, 500, 0 };
	leftWall->rotate({ 0, 0, 1.5 * M_PI });

	rightWall->position = { 500, 0, 0 };
	rightWall->rotate({ 0, 0, M_PI / 2 });

	add(leftWall);
	add(rightWall);

	setBackgroundColor(0, 0, 100);
}
