#include <System/Controller.h>
#include <Scenes/Default.h>
#include <Scenes/LightTest.h>
#include <Engine.h>

int main(int argc, char* argv[]) {
	Engine* engine = new Engine(1200, 720, "Engine Demo", DEBUG_STATS | DEBUG_COMMAND_LINE);
	Controller* demo = new Controller(engine);

	demo->enterScene(new LightTest());
	demo->run();

	delete demo;
	return 0;
}
