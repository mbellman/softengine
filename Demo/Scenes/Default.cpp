#include <Scenes/Default.h>
#include <Scenes/Garden.h>
#include <System/Math.h>
#include <UI/UIObjects.h>
#include <Helpers.h>
#include <cmath>

/**
 * Default
 * -------
 */
void Default::load() {
	Mesh* mesh = new Mesh(100, 40, 50);

	mesh->position = { -1000, 0, -1000 };
	mesh->isStatic = true;
	mesh->isFlatShaded = true;

	mesh->setVertexOffsets([=](int row, int column, Vec3& offset) {
		offset.y = rand() % 50;
	});

	Cube* cube = new Cube(100);
	Cube* cube2 = new Cube(50);
	Cube* cube3 = new Cube(25);
	Cube* oscillatingCube = new Cube(50);

	cube->position = { -200, 200, 500 };
	cube2->position = { 50, 150, 500 };
	cube3->position = { 200, 100, 500 };
	cube->isStatic = true;
	cube2->isStatic = true;
	cube3->isStatic = true;
	oscillatingCube->position = { 0, 0, 1500 };

	cube->rotate({ 0.5, 0.5, 0.5 });
	cube2->rotate({ 1, 1.5, 0.7 });
	cube3->rotate({ -0.5, 0.8, -0.3 });

	ObjLoader icosahedronObj("./DemoAssets/da-vinci.obj");
	Model* icosahedron = new Model(icosahedronObj);

	icosahedron->position = { 0, 220, 2000 };
	icosahedron->scale(200);
	icosahedron->isStatic = true;

	add(mesh);
	add(cube);
	add(cube2);
	add(cube3);
	add("icosahedron", icosahedron);
	add("oscillatingCube", oscillatingCube);

	Light* light = new Light();
	light->setColor(0, 200, 255);
	light->position = { 0, 100, 2000 };
	light->range = 1000;

	add("light", light);

	UIRect* blueRect = new UIRect();

	blueRect->setColor({ 0, 0, 255 });
	blueRect->setSize(200, 50);
	blueRect->setAlpha(0.5f);
	blueRect->position = { 990, 10 };

	ui->add("blueRect", blueRect);

	settings.backgroundColor = { 75, 0, 50 };
	settings.brightness = 0.2;
	settings.ambientLightColor = { 255, 0, 200 };
	settings.ambientLightVector = { 0, -1, 1 };
	settings.ambientLightFactor = 0.6;
}

void Default::onUpdate(int dt, int runningTime) {
	getObject("oscillatingCube")->position.y = 200.0f + 100.0f * sinf(runningTime / 500.0f);

	Light* light = (Light*)getObject("light");
	light->position.x = 500.0f * sinf(runningTime / 400.0f);
	light->position.z = 2000 + 500.0f * cosf(runningTime / 400.0f);

	settings.ambientLightVector.x = sinf(runningTime / 300.f);

	Object* ico = getObject("icosahedron");

	float icoDistance = Vec3::distance(ico->position, camera->position);

	if (icoDistance < 200) {
		ico->position = {
			ico->position.x + RNG::random(-1000.0f, 1000.0f),
			ico->position.y,
			ico->position.z + RNG::random(-1000.0f, 1000.0f)
		};

		controller->enterScene(new Garden());
	}
}
