#include <System/Controller.h>
#include <System/Level.h>
#include <UI/Alert.h>
#include <Engine.h>

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
		levelStack.back()->suspend();

		if (levelStack.size() > 10) {
			Alert::error(ALERT_ERROR, "Level stack size limit exceeded");
			exit(0);
		}
	}

	levelStack.push_back(level);

	level->setController(this);
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

	level->setController(this);
	engine->setActiveLevel(level);
}
