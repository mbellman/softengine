#include <Engine.h>
#include <Levels/Default.h>
#include <Levels/StressTest.h>
#include <Levels/LightTest.h>
#include <Levels/TextureTest.h>
#include <Levels/Garden.h>

int width = 1200;
int height = 720;

int main(int argc, char* argv[]) {
	Engine engine(width, height, DEBUG_DRAWTIME);

	TTF_Font* mono = TTF_OpenFont("./DemoAssets/FreeMono.ttf", 15);

	UIText text("Test Text");

	text.setFont(mono);
	text.setPosition(10, 10);

	engine.addUIObject(&text);
	engine.setActiveLevel(new Garden());
	engine.run();

	TTF_CloseFont(mono);

	return 0;
}
