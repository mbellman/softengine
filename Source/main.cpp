#include <Objects.h>
#include <Engine.h>

int width = 1200;
int height = 720;

int main(int argc, char* argv[]) {
	Engine engine(width, height, SHOW_WIREFRAME | DEBUG_DRAWTIME);

	Mesh mesh(40, 40, 50);

	mesh.position = { -1000, 0, -1000 };
	mesh.setColor(0, 255, 0);

	Cube cube(100);
	Cube cube2(50);
	Cube cube3(25);

	cube.position = { -200, 100, 500 };
	cube2.position = { 50, 100, 500 };
	cube3.position = { 200, 100, 500 };

	engine.addObject(&mesh);
	engine.addObject(&cube);
	engine.addObject(&cube2);
	engine.addObject(&cube3);
	engine.run();

	return 0;
}