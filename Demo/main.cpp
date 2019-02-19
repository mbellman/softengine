#include <Engine.h>
#include <Sound/AudioEngine.h>
#include <Sound/Sound.h>
#include <Levels/Default.h>
#include <Levels/StressTest.h>
#include <Levels/LightTest.h>
#include <Levels/TextureTest.h>
#include <Levels/Garden.h>

int width = 1200;
int height = 720;

int main(int argc, char* argv[]) {
	AudioEngine audio;
	Sound* applause = new Sound("./DemoAssets/applause.wav");
	applause->play();

	Engine engine(width, height, DEBUG_STATS);

	engine.setActiveLevel(new Garden());
	engine.run();

	delete applause;

	return 0;
}
