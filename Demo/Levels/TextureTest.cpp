#include <SDL.h>
#include <math.h>
#include <Levels/TextureTest.h>
#include <System/Objects.h>
#include <Graphics/TextureBuffer.h>

/**
 * TextureTest
 * -----------
 */
void TextureTest::load() {
	add("blockTexture", new TextureBuffer("./DemoAssets/block.png"));

	Cube* cube = new Cube(100);
	cube->position = { 0, 100, 1800 };
	cube->setTexture(getTexture("blockTexture"));
	cube->setFaceUVCoordinates(0.0f, 0.0f, 1.0f, 1.0f);

	add("spinningCube", cube);

	for (int i = 0; i < 6; i++) {
		Cube* c = new Cube(100);

		c->position = {
			300.0f * (i % 2 == 0 ? -1 : 1),
			-150,
			500.0f + 300.0f * (int)(i / 2)
		};

		add(c);
	}

	settings.backgroundColor = { 0, 0, 50 };
	settings.albedo = 0.2;
	settings.ambientLightColor = { 0, 0, 255 };
	settings.ambientLightFactor = 0.7;
	settings.ambientLightVector = { 0.5, -1, 0.3 };
}

void TextureTest::update(int dt, int runningTime) {
	// getObject("spinningCube")->rotate({ 0.02, -0.03, 0 });
}

