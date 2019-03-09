#pragma once

#include <System/Scene.h>
#include <SDL.h>

class Default : public Scene {
public:
	void load() override;
	void onUpdate(int dt) override;

private:
	void onKeyDown(const SDL_Keycode& code);
	void onKeyUp(const SDL_Keycode& code);
};
