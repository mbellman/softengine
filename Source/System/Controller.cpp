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
	pendingScene = scene;
	pendingSceneChange = SceneChange::ENTER_SCENE;
}

void Controller::exitScene() {
	pendingSceneChange = SceneChange::EXIT_SCENE;
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

void Controller::handleEnterScene() {
	if (sceneStack.size() > 0) {
		sceneStack.back()->suspend();

		if (sceneStack.size() > 10) {
			Alert::error(ALERT_ERROR, "Scene stack size limit exceeded");
			exit(0);
		}
	}

	sceneStack.push_back(pendingScene);
	pendingScene->provideController(this);
	engine->setActiveScene(pendingScene);
}

void Controller::handleExitScene() {
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

void Controller::handlePendingSceneChange() {
	switch (pendingSceneChange) {
		case SceneChange::ENTER_SCENE:
			handleEnterScene();
			break;
		case SceneChange::EXIT_SCENE:
			handleExitScene();
			break;
		case SceneChange::SWITCH_SCENE:
			handleSwitchScene();
			break;
	}

	pendingScene = nullptr;
	pendingSceneChange = SceneChange::NONE;
}

void Controller::handleSwitchScene() {
	delete sceneStack.back();

	sceneStack.pop_back();
	sceneStack.push_back(pendingScene);

	pendingScene->provideController(this);
	engine->setActiveScene(pendingScene);
}

bool Controller::isMouseCaptured() {
	return SDL_GetRelativeMouseMode();
}

void Controller::start(Scene* scene) {
	enterScene(scene);

	engine->initialize();

	int dt, lastStartTime = (int)SDL_GetTicks();

	while (!engine->hasStopped()) {
		dt = (int)SDL_GetTicks() - lastStartTime;

		if (pendingSceneChange != SceneChange::NONE) {
			handlePendingSceneChange();
		}

		if (sceneStack.size() > 0) {
			Scene* activeScene = sceneStack.back();

			if (!activeScene->hasInitialized) {
				// If we get here, the Scene was reset without
				// a Scene change, requiring that we reload/restart
				// it before continuing the update cycle.
				engine->setActiveScene(activeScene);
			}
		}

		lastStartTime = (int)SDL_GetTicks();

		engine->update(dt);
	}
}

void Controller::switchScene(Scene* scene) {
	pendingScene = scene;
	pendingSceneChange = SceneChange::SWITCH_SCENE;
}

void Controller::toggleFlag(Flags flag) {
	engine->toggleFlag(flag);
}
