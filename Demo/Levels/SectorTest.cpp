#include <Levels/SectorTest.h>
#include <System/Objects.h>
#include <System/Geometry.h>
#include <Helpers.h>

/**
 * SectorTest
 * ----------
 */
void SectorTest::load() {
	Sector sector1;

	sector1.bounds.cornerA = { -1000.0f, 0.0f, 500.0f };
	sector1.bounds.cornerB = { 1500.0f, 500.0f, 3000.0f };
	sector1.id = 1;

	add(sector1);

	Sector sector2;

	sector2.bounds.cornerA = { 1000.0f, 0.0f, 2500.0f };
	sector2.bounds.cornerB = { 3000.0f, 500.0f, 4000.0f };
	sector2.id = 2;

	add(sector2);

	Mesh* mesh1 = new Mesh(8, 10, 200);
	mesh1->position = sector1.bounds.cornerA;
	mesh1->setColor({ 200, 0, 0 });
	mesh1->isStatic = true;

	add(mesh1);

	Mesh* mesh2 = new Mesh(8, 10, 200);
	mesh2->position = sector2.bounds.cornerA;
	mesh2->setColor({ 0, 0, 200 });
	mesh2->isStatic = true;

	add(mesh2);

	// Sector 1 cubes
	for (int i = 0; i < 5; i++) {
		Cube* cube = new Cube(100);
		cube->setColor({ 200, 25, 100 });
		cube->isStatic = true;
		cube->sectorId = 1;

		cube->position = {
			RNG::random(-1000, 1000),
			RNG::random(50, 500),
			RNG::random(500, 2000),
		};

		cube->rotateDeg({
			RNG::random(0.0f, 360.0f),
			RNG::random(0.0f, 360.0f),
			RNG::random(0.0f, 360.0f)
		});

		add(cube);

		Light* light = new Light();
		light->setColor({ 0, 50, 255 });
		light->isStatic = true;
		light->range = 750;

		light->position = {
			cube->position.x + RNG::random(-300, 300),
			cube->position.y + RNG::random(-300, 300),
			cube->position.z + RNG::random(-300, 300)
		};

		add(light);
	}

	// Sector 2 cubes
	for (int i = 0; i < 5; i++) {
		Cube* cube = new Cube(100);
		cube->setColor({ 25, 150, 200 });
		cube->isStatic = true;
		cube->sectorId = 2;

		cube->position = {
			2000.0f + RNG::random(-1000, 1000),
			RNG::random(50, 500),
			2000.0f + RNG::random(500, 2000),
		};

		cube->rotateDeg({
			RNG::random(0.0f, 360.0f),
			RNG::random(0.0f, 360.0f),
			RNG::random(0.0f, 360.0f)
		});

		add(cube);

		Light* light = new Light();
		light->setColor({ 255, 100, 50 });
		light->isStatic = true;
		light->range = 750;

		light->position = {
			cube->position.x + RNG::random(-300, 300),
			cube->position.y + RNG::random(-300, 300),
			cube->position.z + RNG::random(-300, 300)
		};

		add(light);
	}

	settings.ambientLightColor = { 50, 255, 100 };
	settings.ambientLightFactor = 0.5f;
	settings.ambientLightVector = { -1, -1, 1 };
	settings.hasStaticAmbientLight = true;
	settings.brightness = 0.2f;
}
