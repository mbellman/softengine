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
	bool isMouseFocused();
	void run();
	void switchScene(Scene* scene);
	void toggleFlag(Flags flag);

private:
	Engine* engine = nullptr;
	std::vector<Scene*> sceneStack;
};
