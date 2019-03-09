#include <System/Controller.h>
#include <System/Scene.h>
#include <System/Flags.h>
#include <System/Math.h>
#include <UI/Alert.h>
#include <Engine.h>
#include <SDL.h>

/**
 * Controller
 * ----------
 */
Controller::Controller(Engine* engine) {
	this->engine = engine;
}

Controller::~Controller() {
	for (auto* scene : sceneStack) {
		delete scene;
	}

	sceneStack.clear();

	delete engine;
}

void Controller::enterScene(Scene* scene) {
	if (sceneStack.size() > 0) {
		sceneStack.back()->suspend();

		if (sceneStack.size() > 10) {
			Alert::error(ALERT_ERROR, "Scene stack size limit exceeded");
			exit(0);
		}
	}

	sceneStack.push_back(scene);

	scene->provideController(this);
	engine->setActiveScene(scene);
}

void Controller::exitScene() {
	delete sceneStack.back();

	sceneStack.pop_back();

	if (sceneStack.empty()) {
		engine->stop();
	} else {
		Scene* scene = sceneStack.back();

		scene->resume();
		engine->setActiveScene(scene);
	}
}

int Controller::getFlags() {
	return engine->getFlags();
}

Coordinate Controller::getMousePosition() {
	Coordinate mousePosition;

	SDL_GetMouseState(&mousePosition.x, &mousePosition.y);

	return mousePosition;
}

int Controller::getWindowHeight() {
	return engine->getWindowHeight();
}

int Controller::getWindowWidth() {
	return engine->getWindowWidth();
}

bool Controller::isMouseFocused() {
	return SDL_GetRelativeMouseMode();
}

void Controller::run() {
	engine->run();
}

void Controller::switchScene(Scene* scene) {
	delete sceneStack.back();

	sceneStack.pop_back();
	sceneStack.push_back(scene);

	scene->provideController(this);
	engine->setActiveScene(scene);
}

void Controller::toggleFlag(Flags flag) {
	engine->toggleFlag(flag);
}
