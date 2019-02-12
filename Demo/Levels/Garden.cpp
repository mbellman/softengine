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

	for (int i = 0; i < 15; i++) {
		Model* tree = new Model(treeObj);

		tree->position = { (float)(1000 - rand() % 2000), -10.0f, (float)(3000 - rand() % 2000) };
		tree->setTexture(getTexture("tree-texture"));
		tree->scale(100);

		add(tree);
	}

	for (int x = 0; x < 4; x++) {
		Light* light = new Light();

		light->setColor(255, 200, 150);
		light->position = { (float)(1000 - rand() % 2000), (float)(rand() % 200), (float)(3000 - rand() % 2000) };
		light->spread = 300 + rand() % 700;

		add(light);
	}

	Model* teapot = new Model(teapotObj);
	teapot->setColor(255, 255, 255);
	teapot->position = { 0, 0, 2000 };
	teapot->scale(50);

	add(teapot);

	Mesh* mesh = new Mesh(50, 50, 100);
	mesh->setColor(10, 5, 0);
	mesh->position = { -2500, -40, 0 };
	add(mesh);

	settings.backgroundColor = { 10, 30, 50 };
	settings.drawDistance = 2000;
	settings.albedo = 0.1;
	settings.ambientLightColor = { 0, 10, 40 };
	settings.ambientLightVector = { 0.5, -1, 0.5 };
	settings.ambientLightFactor = 0.7;
}

void Garden::update(int dt, int runningTime) {

}
