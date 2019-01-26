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

	cube.position = { 0, 100, 1000 };

	// engine.addObject(&mesh);
	engine.addObject(&cube);
	engine.run();

	return 0;
}