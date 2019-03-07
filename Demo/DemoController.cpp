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
void DemoController::loadAlternateLevel() {
	switchLevel(new Beach());
}

void DemoController::loadDefaultLevel() {
	enterLevel(new Garden());
}

void DemoController::start() {
	loadDefaultLevel();
	run();
}
