#pragma once

#include <SDL.h>
#include <functional>

typedef std::function<void(int, int)> MouseMotionHandler;
typedef std::function<void(const SDL_Keycode&)> KeyPressHandler;

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
	void onKeyPress(KeyPressHandler handler);
	void onMouseMotion(MouseMotionHandler handler);
	void resetKeyState();

private:
	int keyState = 0;
	KeyPressHandler keyPressHandler = nullptr;
	MouseMotionHandler mouseMotionHandler = nullptr;

	void handleKeyDown(const SDL_Keycode& code);
	void handleKeyUp(const SDL_Keycode& code);
	void handleMouseMotionEvent(const SDL_MouseMotionEvent& event);
};
