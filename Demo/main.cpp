#include <System/Controller.h>
#include <Scenes/Default.h>
#include <Scenes/StressTest.h>
#include <Scenes/LightTest.h>
#include <Scenes/TextureTest.h>
#include <Scenes/Garden.h>
#include <Scenes/Beach.h>
#include <Scenes/SectorTest.h>
#include <Scenes/AnimationTest.h>
#include <Engine.h>

int main(int argc, char* argv[]) {
	Engine* engine = new Engine(
		1200, 720,
		"Engine Demo",
		"./DemoAssets/icon.png",
		"./DemoAssets/FreeMono.ttf",
		DEBUG_STATS | DEBUG_COMMAND_LINE
	);

	Controller* demo = new Controller(engine);

	demo->start(new Default());

	delete demo;
	return 0;
}
