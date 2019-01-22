#include <Objects.h>
#include <Engine.h>

int width = 1200;
int height = 720;

int main(int argc, char* argv[]) {
	Engine engine;
	Mesh mesh(10, 10, 50);

	mesh.position = { 100, 0, 100 };
	mesh.setColor(0, 255, 0);

	engine.addObject(&mesh);
	engine.run(width, height);

	return 0;
}