#include <Objects.h>
#include <Engine.h>

int width = 1200;
int height = 720;

int main(int argc, char* argv[]) {
	Engine engine;
	Mesh mesh(5, 5, 50);

	mesh.setPosition(100, 0, 100);

	engine.addObject(&mesh);
	engine.run(width, height);

	return 0;
}