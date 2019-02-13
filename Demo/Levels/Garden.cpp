#include <Levels/Garden.h>
#include <Graphics/TextureBuffer.h>
#include <System/Objects.h>

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

	for (int i = 0; i < 15; i++) {
		Model* tree = new Model(treeObj);

		tree->position = { (float)(1000 - rand() % 2000), -10.0f, (float)(3000 - rand() % 2000) };
		tree->setTexture(getTexture("tree-texture"));
		tree->scale(100);

		add(tree);
	}

	for (int x = 0; x < 6; x++) {
		Light* light = new Light();
		Cube* cube = new Cube(10);
		cube->setColor(255, 255, 255);

		light->setColor(255, 100, 0);
		light->position = { (float)(1000 - rand() % 2000), (float)(rand() % 200), (float)(3000 - rand() % 2000) };
		light->spread = 300 + rand() % 700;

		cube->position.x = light->position.x;
		cube->position.y = light->position.y;
		cube->position.z = light->position.z;

		add(light);
		add(cube);
	}

	Model* teapot = new Model(teapotObj);
	teapot->setColor(255, 255, 255);
	teapot->position = { 0, 0, 2000 };
	teapot->scale(50);

	add(teapot);

	Mesh* mesh = new Mesh(50, 50, 100);
	mesh->setColor(10, 5, 0);
	mesh->position = { -2500, -40, 0 };
	mesh->setTexture(getTexture("ground-texture"));
	mesh->setTextureInterval(5, 5);
	add(mesh);

	settings.backgroundColor = { 0, 0, 0 };
	settings.drawDistance = 4000;
	settings.brightness = 0.2;
	settings.ambientLightColor = { 0, 0, 255 };
	settings.ambientLightVector = { 0, -0.1, 1 };
	settings.ambientLightFactor = 0.8;
}

void Garden::update(int dt, int runningTime) {

}
