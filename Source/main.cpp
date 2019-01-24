#include <Objects.h>
#include <Engine.h>

int width = 1200;
int height = 720;

int main(int argc, char* argv[]) {
	Engine engine(width, height);
	Mesh mesh(20, 20, 50);

	mesh.position = { -500, 0, 1000 };
	mesh.setColor(0, 255, 0);

	engine.addObject(&mesh);
	engine.run();

	return 0;
}