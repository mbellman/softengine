#include <Levels/Garden.h>
#include <Graphics/TextureBuffer.h>
#include <System/Objects.h>
#include <cmath>

/**
 * Garden
 * ------
 */
void Garden::load() {
	ObjLoader treeObj("./DemoAssets/tree-model.obj");
	ObjLoader icoObj("./DemoAssets/da-vinci.obj");
	ObjLoader teapotObj("./DemoAssets/teapot.obj");

	add("tree-texture", new TextureBuffer("./DemoAssets/tree-texture.png"));
	add("ground-texture", new TextureBuffer("./DemoAssets/ground-texture.png"));

	for (int i = 0; i < 30; i++) {
		Model* tree = new Model(treeObj);

		tree->position = { (float)(2000 - rand() % 4000), -10.0f, (float)(5000 - rand() % 4000) };
		tree->setTexture(getTexture("tree-texture"));
		tree->scale(100);
		tree->rotateDeg({ 0, (float)(rand() % 360), 0 });
		tree->isStatic = true;

		add(tree);
	}

	for (int x = 0; x < 10; x++) {
		Light* light = new Light();
		Cube* cube = new Cube(10);
		cube->setColor(255, 255, 255);

		light->setColor(255, 50, 0);
		light->position = { (float)(2000 - rand() % 4000), 50 + (float)(rand() % 200), (float)(5000 - rand() % 4000) };
		light->range = 500 + rand() % 700;
		light->power = 1.5f;
		light->isStatic = true;

		cube->position.x = light->position.x;
		cube->position.y = light->position.y;
		cube->position.z = light->position.z;
		cube->isStatic = true;

		add(light);
		add(cube);
	}

	Light* movingLight = new Light();
	movingLight->setColor({ 255, 50, 255 });
	movingLight->range = 750;
	movingLight->position.y = 300;
	movingLight->power = 1.5f;

	add("movingLight", movingLight);

	Model* teapot = new Model(teapotObj);
	teapot->setColor(255, 255, 255);
	teapot->position = { -1000, 10, 2000 };
	teapot->scale(50);
	teapot->isStatic = true;

	add(teapot);

	Mesh* mesh = new Mesh(50, 50, 100);
	mesh->setColor(10, 5, 0);
	mesh->position = { -2500, -40, 0 };
	mesh->setTexture(getTexture("ground-texture"));
	mesh->setTextureInterval(5, 5);
	mesh->isStatic = true;

	add(mesh);

	settings.backgroundColor = { 0, 0, 0 };
	// settings.visibility = 4000;
	settings.brightness = 0.2;
	settings.ambientLightColor = { 0, 0, 255 };
	settings.ambientLightVector = { 0, -0.1, 1 };
	settings.ambientLightFactor = 1;
	settings.hasStaticAmbientLight = true;
}

void Garden::update(int dt, int runningTime) {
	Light* movingLight = (Light*)getObject("movingLight");

	movingLight->position.x = 1500.0f * sinf(runningTime / 900.0f);
	movingLight->position.z = 2000.0f + 1500.0f * cosf(runningTime / 900.0f);
	movingLight->position.y = 300.0f + 150.0f * sinf(runningTime / 500.0f);
}
