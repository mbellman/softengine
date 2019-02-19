#include <cmath>

#include <Levels/Garden.h>
#include <Graphics/TextureBuffer.h>
#include <Sound/Sound.h>
#include <System/Objects.h>
#include <System/ParticleSystem.h>

/**
 * Garden
 * ------
 */
void Garden::load() {
	ObjLoader treeObj("./DemoAssets/tree-model.obj");
	ObjLoader icoObj("./DemoAssets/da-vinci.obj");

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

	Model* icosahedron = new Model(icoObj);
	icosahedron->setColor(255, 255, 255);
	icosahedron->position = { 0, 150, 2500 };
	icosahedron->scale(200);
	icosahedron->isStatic = true;

	add(icosahedron);

	Mesh* mesh = new Mesh(50, 50, 100);
	mesh->setColor(10, 5, 0);
	mesh->position = { -2500, -40, 0 };
	mesh->setTexture(getTexture("ground-texture"));
	mesh->setTextureInterval(5, 5);
	mesh->isStatic = true;

	add(mesh);

	ParticleSystem* snow = new ParticleSystem(2000);

	snow->setSpawnRange(
		{ -2500.0f, 2500.0f },
		{ 0.0f, 2000.0f },
		{ 0.0f, 5000.0f }
	);

	snow->setParticleColor({ 255, 255, 255 });
	snow->setParticleSize(5, 5);

	snow->setParticleBehavior([=](Particle* particle, int dt) {
		particle->position.y -= (float)dt / 5.0f;

		if (particle->position.y < -100) {
			particle->shouldReset = true;
		}
	});

	addParticleSystem("snow", snow);

	auto* applause = new Sound("./DemoAssets/applause.wav");
	applause->setPosition({0, 0, 0});
	applause->play();

	settings.backgroundColor = { 0, 10, 20 };
	settings.visibility = 3500;
	settings.brightness = 0.1;
	settings.ambientLightColor = { 0, 0, 100 };
	settings.ambientLightVector = { 0, -0.8f, 0.5f };
	settings.ambientLightFactor = 0.5f;
	settings.hasStaticAmbientLight = true;
}

void Garden::onUpdate(int dt, int runningTime) {
	auto* movingLight = getObject("movingLight");

	movingLight->position.x = 1500.0f * sinf(runningTime / 900.0f);
	movingLight->position.z = 2000.0f + 1500.0f * cosf(runningTime / 900.0f);
	movingLight->position.y = 300.0f + 150.0f * sinf(runningTime / 500.0f);
}
