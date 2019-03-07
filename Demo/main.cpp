#include <DemoController.h>
#include <Engine.h>

int main(int argc, char* argv[]) {
	Engine* engine = new Engine(1200, 720, DEBUG_STATS | DEBUG_COMMAND_LINE);
	DemoController* demo = new DemoController(engine);

	demo->start();

	delete demo;
	return 0;
}
