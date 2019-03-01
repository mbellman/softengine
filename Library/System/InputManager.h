#pragma once

#include <SDL.h>
#include <functional>

typedef std::function<void(int, int)> MouseMotionHandler;

/**
 * Keys
 * ----
 */
enum Keys {
	W = 1 << 0,
	A = 1 << 1,
	S = 1 << 2,
	D = 1 << 3,
	SHIFT = 1 << 4
};

/**
 * InputManager
 * ------------
 */
class InputManager {
public:
	void handleEvent(const SDL_Event& event);
	bool isKeyPressed(Keys key);
	void onMouseMotion(MouseMotionHandler handler);

private:
	int keyState = 0;
	MouseMotionHandler mouseMotionHandler;

	void handleKeyDown(const SDL_Keycode& code);
	void handleKeyUp(const SDL_Keycode& code);
	void handleMouseMotionEvent(const SDL_MouseMotionEvent& event);
};
