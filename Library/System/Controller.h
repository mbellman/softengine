#pragma once

#include <vector>

class Engine;
class Level;

class Controller {
public:
	Controller(Engine* engine);
	~Controller();

	void enterLevel(Level* level);
	void exitLevel();
	void run();
	void switchLevel(Level* level);

private:
	Engine* engine = nullptr;
	std::vector<Level*> levelStack;
};
