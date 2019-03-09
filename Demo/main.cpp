#include <System/Controller.h>
#include <Scenes/Default.h>
#include <Scenes/LightTest.h>
#include <Scenes/AnimationTest.h>
#include <Scenes/Beach.h>
#include <Engine.h>

int main(int argc, char* argv[]) {
	Engine* engine = new Engine(
		1200, 720,
		"Engine Demo",
		"./DemoAssets/icon.png",
		"./DemoAssets/FreeMono.ttf",
		DEBUG_STATS | DEBUG_COMMAND_LINE
	);

	engine->lockRasterArea(100, 0, 1000, 600);

	Controller* demo = new Controller(engine);

	demo->enterScene(new Default());
	demo->run();

	delete demo;
	return 0;
}
