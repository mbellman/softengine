#pragma once

#include <System/Scene.h>
#include <SDL.h>

class Default : public Scene {
public:
	void load() override;
	void onUpdate(int dt, int runningTime) override;

private:
	bool isPaused = false;

	void onKeyPress(const SDL_Keycode& code);
};
