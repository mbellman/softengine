#include <DemoController.h>
#include <Levels/Default.h>
#include <Levels/StressTest.h>
#include <Levels/LightTest.h>
#include <Levels/TextureTest.h>
#include <Levels/Garden.h>
#include <Levels/Beach.h>
#include <Levels/SectorTest.h>
#include <Levels/AnimationTest.h>

/**
 * DemoController
 * --------------
 */
void DemoController::loadDefaultLevel() {
	loadLevel(new Default());
}

void DemoController::loadAlternateLevel() {
	loadLevel(new Beach());
}

void DemoController::start() {
	loadDefaultLevel();
}
