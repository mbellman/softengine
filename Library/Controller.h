#pragma once

#include <Engine.h>
#include <System/Level.h>

class Controller {
public:
	Controller(Engine* engine);
	~Controller();

	virtual void start() = 0;

protected:
	void loadLevel(Level* level);

private:
	Engine* engine = nullptr;
	Level* activeLevel = nullptr;
};
