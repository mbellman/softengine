#include <System/Objects.h>
#include <Loaders/ObjLoader.h>
#include <Levels/StressTest.h>

/**
 * StressTest
 * ----------
 */
void StressTest::load() {
	ObjLoader icosahedronObj("./DemoAssets/da-vinci.obj");

	Mesh* mesh = new Mesh(400, 80, 50);
	mesh->position = { -2000, 0, 0 };

	add(mesh);

	for (int i = 0; i < 10; i++) {
		Model* icosahedron = new Model(icosahedronObj);
		Cube* cube = new Cube(100);

		icosahedron->position = { -1000, 50, 500.0f + (i * 1000) };
		icosahedron->scale(200);

		cube->position = { 500, 180, 500.0f + (i * 500) };
		cube->rotate({ 0, i * 0.5f, -i * 0.5f });

		add(icosahedron);
		add(cube);
	}

	for (int i = 0; i < 5; i++) {
		Light* light = new Light();
		float x = i % 2 == 0 ? -500 : 500;
		float y = 200;
		float z = i * 1200;

		light->position = { x, y, z };
		light->color = { 255, 0, 0 };
		light->spread = 900;

		add(light);
	}

	settings.drawDistance = 6000;
	settings.albedo = 0.2;
	settings.ambientLightColor = { 0, 100, 0 };
	settings.ambientLightVector = { 0, -1, 1 };
	settings.ambientLightFactor = 0.2;
	settings.backgroundColor = { 0, 100, 200 };
}

