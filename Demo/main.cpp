#include <Engine.h>
#include <Sound/AudioEngine.h>
#include <Levels/Default.h>
#include <Levels/StressTest.h>
#include <Levels/LightTest.h>
#include <Levels/TextureTest.h>
#include <Levels/Garden.h>

int width = 1200;
int height = 720;

int main(int argc, char* argv[]) {
	AudioEngine audio;
	audio.init();

	Engine engine(width, height, DEBUG_STATS);

	engine.setActiveLevel(new Garden());
	engine.run();

	return 0;
}
