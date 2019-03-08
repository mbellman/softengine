#pragma once

#include <vector>

class Engine;
class Scene;

class Controller {
public:
	Controller(Engine* engine);
	~Controller();

	void enterScene(Scene* scene);
	void exitScene();
	void run();
	void switchScene(Scene* scene);

private:
	Engine* engine = nullptr;
	std::vector<Scene*> sceneStack;
};
