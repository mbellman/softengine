#include <Levels/Default.h>

/**
 * Default
 * -------
 */
void Default::load() {
	Mesh* mesh = new Mesh(100, 40, 50);

	mesh->position = { -1000, 0, -1000 };

	Cube* cube = new Cube(100);
	Cube* cube2 = new Cube(50);
	Cube* cube3 = new Cube(25);

	cube->position = { -200, 200, 500 };
	cube2->position = { 50, 150, 500 };
	cube3->position = { 200, 100, 500 };

	cube->rotate({ 0.5, 0.5, 0.5 });
	cube2->rotate({ 1, 1.5, 0.7 });
	cube3->rotate({ -0.5, 0.8, -0.3 });

	ObjLoader icosahedronObj("./DemoAssets/da-vinci.obj");
	Model* icosahedron = new Model(icosahedronObj);

	icosahedron->position = { 0, 50, 2000 };
	icosahedron->scale(200);

	add(mesh);
	add(cube);
	add(cube2);
	add(cube3);
	add(icosahedron);
}
