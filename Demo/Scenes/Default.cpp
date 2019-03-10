#include <Scenes/Default.h>
#include <Scenes/Garden.h>
#include <System/Math.h>
#include <System/Flags.h>
#include <UI/UIObjects.h>
#include <Helpers.h>
#include <cmath>
#include <SDL.h>

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

	UIGraphic* icon = new UIGraphic("./DemoAssets/icon.png");

	ui->add("icon", icon);

	UIRect* blueRect = new UIRect();

	blueRect->setColor({ 0, 0, 255 });
	blueRect->setSize(200, 50);
	blueRect->setAlpha(0.5f);
	blueRect->position = { 990, 10 };

	ui->add("blueRect", blueRect);

	UIGraphic* hud = new UIGraphic("./DemoAssets/hud.png");

	hud->position = { 1000, 100 };
	hud->setAlpha(0.5f);
	hud->clip(120, 59);

	ui->add("hud", hud);

	inputManager->onKeyDown([=](const SDL_Keycode& code) {
		onKeyDown(code);
	});

	inputManager->onKeyUp([=](const SDL_Keycode& code) {
		onKeyUp(code);
	});

	inputManager->onMouseClick([=]() {
		Vec3 cameraDirection = camera->getDirection();

		Cube* spawnedCube = new Cube(30);

		spawnedCube->setColor({ 255, 0, 255 });

		spawnedCube->position = {
			camera->position.x + cameraDirection.x * 750.0f,
			camera->position.y + cameraDirection.y * 750.0f,
			camera->position.z + cameraDirection.z * 750.0f
		};

		spawnedCube->rotateDeg({
			RNG::random(0.0f, 90.0f),
			RNG::random(0.0f, 90.0f),
			RNG::random(0.0f, 90.0f)
		});

		spawnedCube->lifetime = 2000;

		Light* cubeLight = new Light();
		cubeLight->setColor({ 0, 255, 0 });
		cubeLight->lifetime = 2000;
		cubeLight->position = spawnedCube->position;

		add(spawnedCube);
		add(cubeLight);
	});

	settings.backgroundColor = { 75, 0, 50 };
	settings.brightness = 0.2;
	settings.ambientLightColor = { 255, 0, 200 };
	settings.ambientLightVector = { 0, -1, 1 };
	settings.ambientLightFactor = 0.6;
}

void Default::onKeyDown(const SDL_Keycode& code) {
	if (code == SDLK_q) {
		camera->roll -= 0.01f;
	} else if (code == SDLK_e) {
		camera->roll += 0.01f;
	}
}

void Default::onKeyUp(const SDL_Keycode& code) {
	if (code == SDLK_p) {
		togglePause();
	} else if (code == SDLK_f) {
		controller->toggleFlag(PIXEL_FILTER);
	} else if (code == SDLK_m) {
		controller->toggleFlag(DISABLE_WINDOW_RESIZE);
	}
}

void Default::onUpdate(int dt) {
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

	if (controller->isMouseCaptured()) {
		Coordinate mousePosition = controller->getMousePosition();

		ui->get("icon")->position.x = mousePosition.x;
		ui->get("icon")->position.y = mousePosition.y;
	}

	int runningTime = getRunningTime();
	int hudXClip = (int)(191.0f * (1.0f + sinf(runningTime / 500.0f)) / 2.0f);
	int blueRectXClip = (int)(200.0f * (1.0f + cosf(runningTime / 500.0f)) / 2.0f);

	UIGraphic* hud = (UIGraphic*)ui->get("hud");

	hud->clip(hudXClip, 59);
	hud->position.x = controller->getWindowWidth() - 200;

	Object* oscillatingCube = getObject("oscillatingCube");
	Light* light = (Light*)getObject("light");
	UIRect* blueRect = (UIRect*)ui->get("blueRect");

	if (runningTime > 5000) {
		if (oscillatingCube != NULL) {
			remove("oscillatingCube");
			remove("light");

			ui->remove("blueRect");
		}
	} else {
		oscillatingCube->position.y = 200.0f + 100.0f * sinf(runningTime / 500.0f);

		light->position.x = 500.0f * sinf(runningTime / 400.0f);
		light->position.z = 2000 + 500.0f * cosf(runningTime / 400.0f);

		settings.ambientLightVector.x = sinf(runningTime / 300.f);

		blueRect->clip(blueRectXClip, 50);
		blueRect->position.x = controller->getWindowWidth() - 220;
	}
}
