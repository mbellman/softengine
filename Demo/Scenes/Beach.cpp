#include <Scenes/Beach.h>
#include <System/Objects.h>
#include <SDL.h>
#include <cmath>
#include <stdlib.h>

/**
 * Beach
 * -----
 */
void Beach::load() {
	TextureBuffer* sky = new TextureBuffer("./DemoAssets/sky.png");

	Skybox* skybox = new Skybox(20000.0f);
	skybox->setTexture(sky);
	skybox->lockTo(camera);

	add(skybox);
	add("skytex", sky);

	Mesh* water = new Mesh(75, 75, 200);
	water->setColor({ 20, 125, 200 });
	water->position = { -7500, -1000, 3000 };
	water->fresnelFactor = 2.0f;

	add("water", water);

	Mesh* sand = new Mesh(25, 50, 200);
	sand->position = { -5000, -200, 0 };
	sand->setColor({ 150, 140, 80 });
	sand->isStatic = true;

	sand->setVertexOffsets([=](int row, int column, Vec3& offset) {
		offset.y = -(row * 40.0f + rand() % 100);
	});

	add(sand);

	Light* sun = new Light();

	sun->setColor({ 255, 150, 100 });
	sun->position = { 0, 0, 16000 };
	sun->isStatic = true;
	sun->range = 15000;
	sun->power = 1.0f;

	sun->follow(camera, [=](const Vec3& cameraPosition, Vec3& sunPosition) {
		sunPosition.x = cameraPosition.x;
	});

	add(sun);

	settings.backgroundColor = { 115, 75, 170 };
	settings.ambientLightColor = { 255, 175, 50 };
	settings.ambientLightVector = { 0, -0.25, -0.75 };
	settings.brightness = 0.2;
	settings.hasStaticAmbientLight = true;
}

void Beach::onUpdate(int dt) {
	Mesh* water = (Mesh*)getObject("water");

	water->setVertexOffsets([=](int row, int column, Vec3& offset) {
		offset.y = 40.0f * sinf(row / 1.5f + getRunningTime() / 600.0f) + 40.0f * sinf((row * M_PI) / 2.0f + column / 1.5f + getRunningTime() / 600.0f);
	});
}
