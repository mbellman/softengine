#include <SDL.h>
#include <math.h>
#include <Scenes/LightTest.h>
#include <System/Objects.h>
#include <Loaders/ObjLoader.h>
#include <Graphics/TextureBuffer.h>

/**
 * LightTest
 * ---------
 */
void LightTest::load() {
	ObjLoader icoObj("./DemoAssets/da-vinci.obj");

	Mesh* floorMesh = new Mesh(20, 70, 50);
	Mesh* leftWall = new Mesh(5, 35, 100);
	Mesh* rightWall = new Mesh(5, 35, 100);

	floorMesh->position = { 500, 0, 0 };
	floorMesh->rotateDeg({ 0, -90, 0 });
	floorMesh->isStatic = true;

	floorMesh->setVertexOffsets([=](int row, int column, Vec3& offset) {
		offset.y = -25 + rand() % 50;
	});

	leftWall->setColor(210, 210, 210);
	leftWall->position = { -500, 0, 0 };
	leftWall->rotateDeg({ 0, -90, 0 });
	leftWall->rotateDeg({ 0, 0, -90 });
	leftWall->isStatic = true;

	leftWall->setVertexOffsets([=](int row, int column, Vec3& offset) {
		offset.x = rand() % 50;
	});

	rightWall->position = { 500, 500, 0 };
	rightWall->rotateDeg({ 0, -90, 90 });
	rightWall->setColor(210, 210, 210);
	rightWall->isStatic = true;

	rightWall->setVertexOffsets([=](int row, int column, Vec3& offset) {
		offset.x = -(rand() % 50);
	});

	add("wall", new TextureBuffer("./DemoAssets/wall.png"));

	leftWall->setTexture(getTexture("wall"));
	leftWall->setTextureInterval(5, 5);
	rightWall->setTexture(getTexture("wall"));
	rightWall->setTextureInterval(5, 5);

	add(floorMesh);
	add(leftWall);
	add(rightWall);

	Model* icosahedron = new Model(icoObj);
	icosahedron->position = { 0, 250, 500 };
	icosahedron->scale(100);
	icosahedron->isStatic = true;

	add(icosahedron);

	Cube* cube1 = new Cube(50);
	Cube* cube2 = new Cube(70);
	Cube* cube3 = new Cube(150);

	cube1->position = { -200, 300, 1000 };
	cube2->position = { 200, 150, 1800 };
	cube3->position = { -150, 200, 2600 };

	cube1->rotate({ -2.4, 0, -1.2 });
	cube2->rotate({ 0, 1.5, 0.6 });
	cube3->rotate({ 1.5, -1.8, 1.6 });

	cube1->isStatic = true;
	cube2->isStatic = true;
	cube3->isStatic = true;

	add("cat", new TextureBuffer("./DemoAssets/cat.png"));
	add("opossum", new TextureBuffer("./DemoAssets/opossum.png"));
	cube1->setTexture(getTexture("opossum"));
	cube1->setFaceUVCoordinates(0.0f, 0.0f, 1.0f, 1.0f);
	cube2->setTexture(getTexture("cat"));
	cube2->setFaceUVCoordinates(0.0f, 0.0f, 1.0f, 1.0f);
	cube3->setTexture(getTexture("cat"));
	cube3->setFaceUVCoordinates(0.0f, 0.0f, 1.0f, 1.0f);

	add(cube1);
	add(cube2);
	add(cube3);

	Light* yellowLight = new Light();
	yellowLight->position = { 0, 300, 500 };
	yellowLight->setColor(255, 220, 50);
	yellowLight->range = 1000;

	Light* light2 = new Light();
	light2->position = { 200, 200, 1500 };
	light2->setColor(255, 0, 0);
	light2->range = 700;
	light2->power = 1.5f;
	light2->isStatic = true;

	Light* light3 = new Light();
	light3->position = { -200, 150, 2200 };
	light3->setColor(0, 0, 255);
	light3->range = 600;
	light3->power = 1.5f;
	light3->isStatic = true;

	Light* light4 = new Light();
	light4->position = { 300, 150, 2900 };
	light4->setColor(0, 255, 0);
	light4->range = 800;
	light4->power = 1.5f;
	light4->isStatic = true;

	add("yellowLight", yellowLight);
	add(light2);
	add(light3);
	add(light4);

	settings.visibility = 4000;
	settings.brightness = 0.1;
	settings.ambientLightColor = { 0, 0, 255 };
	settings.ambientLightVector = { 0, -0.5, 1 };
	settings.ambientLightFactor = 0.2;
	settings.hasStaticAmbientLight = true;
}

void LightTest::onUpdate(int dt) {
	Light* light = (Light*)getObject("yellowLight");

	light->power = 1.0f + cosf(getRunningTime() / 500.0f);
}
