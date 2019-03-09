#pragma once

#include <vector>
#include <functional>
#include <System/Flags.h>
#include <System/Math.h>

class Engine;
class Scene;

class Controller {
public:
	Controller(Engine* engine);
	~Controller();

	void enterScene(Scene* scene);
	void exitScene();
	int getFlags();
	Coordinate getMousePosition();
	int getWindowHeight();
	int getWindowWidth();
	bool isMouseCaptured();
	void start(Scene* scene);
	void switchScene(Scene* scene);
	void toggleFlag(Flags flag);

private:
	enum SceneChange {
		NONE,
		ENTER_SCENE,
		SWITCH_SCENE,
		EXIT_SCENE
	};

	Engine* engine = nullptr;
	Scene* pendingScene = nullptr;
	SceneChange pendingSceneChange = SceneChange::NONE;
	std::vector<Scene*> sceneStack;

	void handleEnterScene();
	void handleExitScene();
	void handlePendingSceneChange();
	void handleSwitchScene();
};
