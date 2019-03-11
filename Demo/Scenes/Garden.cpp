#include <Scenes/Garden.h>
#include <Graphics/TextureBuffer.h>
#include <Sound/Sound.h>
#include <System/Objects.h>
#include <System/ParticleSystem.h>
#include <System/Math.h>
#include <System/Flags.h>
#include <Helpers.h>
#include <cmath>
#include <SDL.h>

/**
 * Garden
 * ------
 */
void Garden::load() {
	ObjLoader treeObj("./DemoAssets/tree-model.obj");
	ObjLoader treeObjLod2("./DemoAssets/tree-model-lod2.obj");
	ObjLoader treeObjLod3("./DemoAssets/tree-model-lod3.obj");
	ObjLoader treeObjLod4("./DemoAssets/tree-model-lod4.obj");

	ObjLoader icoObj("./DemoAssets/da-vinci.obj");
	ObjLoader icoObjLod2("./DemoAssets/da-vinci-lod2.obj");

	add("tree-texture", new TextureBuffer("./DemoAssets/tree-texture.png"));

	for (int i = 0; i < 60; i++) {
		Model* tree = new Model(treeObj);

		tree->addLOD(new Model(treeObjLod2));
		tree->addLOD(new Model(treeObjLod3));
		tree->addLOD(new Model(treeObjLod4));

		tree->position = { (float)(2000 - rand() % 4000), -10.0f, (float)(10000 - rand() % 9000) };
		tree->setTexture(getTexture("tree-texture"));
		tree->scale(100);
		tree->rotateDeg({ 0, (float)(rand() % 360), 0 });
		tree->isStatic = true;

		add(tree);
	}

	for (int x = 0; x < 20; x++) {
		Light* light = new Light();
		Cube* cube = new Cube(10);
		cube->setColor(255, 100, 0);

		light->setColor(255, 100, 0);
		light->position = { (float)(2000 - rand() % 4000), 50 + (float)(rand() % 200), (float)(10000 - rand() % 9000) };
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

	TextureBuffer* snowflakeTexture = new TextureBuffer("./DemoAssets/snowflake.png");
	snowflakeTexture->shouldUseMipmaps = false;

	add("snowflakeTexture", snowflakeTexture);

	ParticleSystem* snow = new ParticleSystem(4000);

	snow->setSpawnRange(
		{ -2500.0f, 2500.0f },
		{ 0.0f, 1200.0f },
		{ -2500.0f, 2500.0f }
	);

	snow->setParticleTexture(getTexture("snowflakeTexture"));
	snow->setParticleSize(5, 5);

	snow->setParticleBehavior([=](Particle* particle, int dt) {
		particle->position.y -= (float)dt / 5.0f;

		if (particle->position.y < -100) {
			particle->shouldReset = true;
		}
	});

	snow->lockTo(camera);

	add("snow", snow);

	Light* movingLight = new Light();
	movingLight->setColor({ 255, 50, 255 });
	movingLight->range = 750;
	movingLight->position.y = 300;
	movingLight->power = 1.5f;

	add("movingLight", movingLight);

	DirectionalLight* directionalLight = new DirectionalLight();

	directionalLight->setColor(255, 0, 0);
	directionalLight->setDirection({ 0, -1, 0 });
	directionalLight->range = 2000;
	directionalLight->position = { 0, 150, 3500 };

	add("directionalLight", directionalLight);

	DirectionalLight* cameraLight = new DirectionalLight();
	cameraLight->range = 1000;
	cameraLight->power = 0.75f;

	cameraLight->lockTo(camera);

	add("cameraLight", cameraLight);

	Model* icosahedron = new Model(icoObj);

	icosahedron->addLOD(new Model(icoObjLod2));
	icosahedron->setColor(255, 255, 255);
	icosahedron->position = { 0, 150, 4000 };
	icosahedron->scale(200);
	icosahedron->isStatic = true;

	add("icosahedron", icosahedron);

	Sound* bells = new Sound("./DemoAssets/bells.wav");
	bells->position = icosahedron->position;
	bells->loop();

	add("bells", bells);

	Mesh* mesh = new Mesh(100, 50, 100);
	mesh->setColor(255, 255, 255);
	mesh->position = { -2500, -40, 0 };
	mesh->isStatic = true;

	mesh->setVertexOffsets([=](int row, int column, Vec3& vertexVector) {
		vertexVector.y = 10.0f * sinf(row) + 10.0f * sinf(column) + rand() % 40;
	});

	add(mesh);

	for (int i = 0; i < 6; i++) {
		Sound* crickets = new Sound("./DemoAssets/crickets.wav");

		crickets->position = {
			RNG::random(-2500.0f, 2500.0f),
			0.0f,
			RNG::random(0.0f, 10000.0f)
		};

		crickets->loop();

		add(crickets);
	}

	inputManager->onKeyUp([=](const SDL_Keycode& code) {
		if (code == SDLK_f) {
			controller->toggleFlag(FPS_30);
		} else if (code == SDLK_g) {
			controller->toggleFlag(SHOW_WIREFRAME);
		}
	});

	settings.backgroundColor = { 0, 10, 20 };
	settings.visibility = 5000;
	settings.brightness = 0.1;
	settings.ambientLightColor = { 0, 0, 100 };
	settings.ambientLightVector = { 0, -0.8f, 0.5f };
	settings.ambientLightFactor = 0.5f;
	settings.hasStaticAmbientLight = true;
}

void Garden::onUpdate(int dt) {
	int runningTime = getRunningTime();
	Light* movingLight = (Light*)getObject("movingLight");

	movingLight->position.x = 1500.0f * sinf(runningTime / 900.0f);
	movingLight->position.z = 2000.0f + 1500.0f * cosf(runningTime / 900.0f);
	movingLight->position.y = 300.0f + 150.0f * sinf(runningTime / 500.0f);

	DirectionalLight* directionalLight = (DirectionalLight*)getObject("directionalLight");
	directionalLight->setDirection({ 0, -1, 2.0f * cosf(runningTime / 600.0f) });

	DirectionalLight* cameraLight = (DirectionalLight*)getObject("cameraLight");
	cameraLight->setDirection({ -sinf(camera->yaw), sinf(camera->pitch), cosf(camera->yaw) });

	if (Vec3::distance(camera->position, getObject("icosahedron")->position) < 200) {
		controller->exitScene();
	}

	// if (runningTime > 5000) {
	// 	ParticleSystem* snow = getParticleSystem("snow");

	// 	if (snow != NULL) {
	// 		remove("snow");
	// 		remove("bells");
	// 	}
	// }
}
