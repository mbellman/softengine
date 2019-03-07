#include <Engine.h>
#include <Levels/Default.h>
#include <Levels/StressTest.h>
#include <Levels/LightTest.h>
#include <Levels/TextureTest.h>
#include <Levels/Garden.h>
#include <Levels/Beach.h>
#include <Levels/SectorTest.h>
#include <Levels/AnimationTest.h>

int width = 1200;
int height = 720;

int main(int argc, char* argv[]) {
	Engine engine(width, height, DEBUG_STATS | DEBUG_COMMAND_LINE);

	engine.setActiveLevel(new Beach());
	engine.run();

	return 0;
}
