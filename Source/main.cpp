#include <Objects.h>
#include <Engine.h>

int width = 1200;
int height = 720;

int main(int argc, char* argv[]) {
	Engine engine(width, height);
	Mesh mesh(10, 10, 50);

	mesh.position = { -250, 0, -250 };
	mesh.setColor(0, 255, 0);

	engine.addObject(&mesh);
	engine.run();

	return 0;
}