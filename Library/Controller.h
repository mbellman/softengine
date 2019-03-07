#pragma once

#include <Engine.h>
#include <System/Level.h>
#include <vector>

class Controller {
public:
	Controller(Engine* engine);
	~Controller();

	virtual void start() = 0;

protected:
	void enterLevel(Level* level);
	void exitLevel();
	void run();
	void switchLevel(Level* level);

private:
	Engine* engine = nullptr;
	std::vector<Level*> levelStack;
};
