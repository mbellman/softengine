#pragma once

#include <vector>
#include <functional>
#include <System/Flags.h>

class Engine;
class Scene;

class Controller {
public:
	Controller(Engine* engine);
	~Controller();

	void enterScene(Scene* scene);
	void exitScene();
	int getFlags();
	int getWindowHeight();
	int getWindowWidth();
	void run();
	void switchScene(Scene* scene);
	void toggleFlag(Flags flag);

private:
	Engine* engine = nullptr;
	std::vector<Scene*> sceneStack;
};
