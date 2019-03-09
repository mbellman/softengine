#include <SDL.h>
#include <math.h>
#include <Scenes/TextureTest.h>
#include <System/Objects.h>
#include <Graphics/TextureBuffer.h>
#include <System/Math.h>
#include <UI/UIObjects.h>

/**
 * TextureTest
 * -----------
 */
void TextureTest::load() {
	TextureBuffer* blockTexture = new TextureBuffer("./DemoAssets/transparent-block.png");
	blockTexture->shouldUseMipmaps = false;

	add("blockTexture", blockTexture);

	Cube* cube = new Cube(100);
	cube->position = { 0, 100, 2500 };
	cube->setTexture(getTexture("blockTexture"));
	cube->setFaceUVCoordinates(0.0f, 0.0f, 1.0f, 1.0f);
	cube->tweenTo({ -1000, 100, 3000 }, 5000, Ease::quadInOut);
	cube->canOccludeSurfaces = false;

	cube->onUpdate = [=](int dt) {
		cube->rotate({ 0.02, -0.03, 0 });
	};

	add("spinningCube", cube);

	for (int i = 0; i < 6; i++) {
		Cube* c = new Cube(100);

		c->position = {
			300.0f * (i % 2 == 0 ? -1 : 1),
			-150,
			2000.0f + 300.0f * (int)(i / 2)
		};

		c->isStatic = true;

		add(c);
	}

	UIGraphic* hud = new UIGraphic("./DemoAssets/hud.png");
	hud->position.x = 1000;
	hud->position.y = 10;

	ui->add("hud", hud);

	settings.backgroundColor = { 0, 0, 50 };
	settings.brightness = 0.2;
	settings.ambientLightColor = { 0, 0, 255 };
	settings.ambientLightVector = { 0.5, -1, 0.3 };
	settings.ambientLightFactor = 0.7;
	settings.hasStaticAmbientLight = true;
}
