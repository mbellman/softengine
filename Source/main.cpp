#include <Loaders/ObjLoader.h>
#include <Objects.h>
#include <Engine.h>

int width = 1200;
int height = 720;

int main(int argc, char* argv[]) {
	Engine engine(width, height, DEBUG_DRAWTIME);

	Mesh mesh(100, 40, 50);

	mesh.position = { -1000, 0, -1000 };
	mesh.setColor(0, 255, 0);

	Cube cube(100);
	Cube cube2(50);
	Cube cube3(25);

	cube.position = { -200, 200, 500 };
	cube2.position = { 50, 150, 500 };
	cube3.position = { 200, 100, 500 };

	cube.rotate({ 0.5, 0.5, 0.5 });
	cube2.rotate({ 1, 1.5, 0.7 });
	cube3.rotate({ -0.5, 0.8, -0.3 });

	ObjLoader icosahedronObj("TestAssets/da-vinci.obj");
	Model icosahedron(icosahedronObj);

	icosahedron.position = { 0, 50, 2000 };
	icosahedron.scale(200);

	engine.addObject(&mesh);
	engine.addObject(&cube);
	engine.addObject(&cube2);
	engine.addObject(&cube3);
	engine.addObject(&icosahedron);
	engine.run();

	return 0;
}
