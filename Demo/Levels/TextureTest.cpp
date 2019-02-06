#include <SDL.h>
#include <math.h>
#include <Levels/TextureTest.h>
#include <Objects.h>

/**
 * TextureTest
 * -----------
 */
void TextureTest::load() {
	Cube* cube = new Cube(100);
	cube->position = { 0, 200, 900 };

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
	settings.albedo = 0.4;
	settings.ambientLightColor = { 0, 0, 255 };
	settings.ambientLightFactor = 0.7;
	settings.ambientLightVector = { 0.5, -1, 1 };
}

void TextureTest::update(int dt, int runningTime) {
	getObject("spinningCube")->rotate({ 0.02, -0.03, 0 });
}

