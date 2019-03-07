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
	for (auto* level : levelStack) {
		delete level;
	}

	levelStack.clear();

	delete engine;
}

void Controller::enterLevel(Level* level) {
	if (levelStack.size() > 0) {
		levelStack.back()->pause();
	}

	levelStack.push_back(level);

	engine->setActiveLevel(level);
}

void Controller::exitLevel() {
	delete levelStack.back();

	levelStack.pop_back();

	if (levelStack.empty()) {
		engine->stop();
	} else {
		Level* level = levelStack.back();

		level->resume();
		engine->setActiveLevel(level);
	}
}

void Controller::run() {
	engine->run();
}

void Controller::switchLevel(Level* level) {
	delete levelStack.back();

	levelStack.pop_back();
	levelStack.push_back(level);

	engine->setActiveLevel(level);
}
