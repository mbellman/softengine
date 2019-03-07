#include <Controller.h>
#include <Engine.h>
#include <System/Level.h>

/**
 * Controller
 * ----------
 */
Controller::Controller(Engine* engine) {
	this->engine = engine;
}

Controller::~Controller() {
	delete engine;
}

void Controller::loadLevel(Level* level) {
	activeLevel = level;

	engine->setActiveLevel(level);
	engine->run();
}
