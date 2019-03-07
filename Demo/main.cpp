#include <DemoController.h>
#include <Engine.h>

int main(int argc, char* argv[]) {
	DemoController* demo = new DemoController(new Engine(1200, 720, DEBUG_STATS | DEBUG_COMMAND_LINE));

	demo->start();

	return 0;
}
