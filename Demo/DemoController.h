#pragma once

#include <Controller.h>
#include <Engine.h>

class DemoController : public Controller {
public:
	DemoController(Engine* engine): Controller(engine) {}

	void start();

private:
	void loadDefaultLevel();
	void loadAlternateLevel();
};
