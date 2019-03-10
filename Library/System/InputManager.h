#pragma once

#include <SDL.h>
#include <functional>

typedef std::function<void(const SDL_Event&)> EventHandler;
typedef std::function<void(int, int)> MouseMotionHandler;
typedef std::function<void()> MouseButtonHandler;
typedef std::function<void(const SDL_Keycode&)> KeyHandler;

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
	void onEvent(EventHandler handler);
	void onKeyDown(KeyHandler handler);
	void onKeyUp(KeyHandler handler);
	void onMouseClick(MouseButtonHandler handler);
	void onMouseDown(MouseButtonHandler handler);
	void onMouseMotion(MouseMotionHandler handler);
	void onMouseUp(MouseButtonHandler handler);
	void resetKeyState();

private:
	int keyState = 0;
	int lastMouseDownTime = 0;
	KeyHandler keyDownHandler = nullptr;
	KeyHandler keyUpHandler = nullptr;
	MouseMotionHandler mouseMotionHandler = nullptr;
	MouseButtonHandler mouseClickHandler = nullptr;
	MouseButtonHandler mouseDownHandler = nullptr;
	MouseButtonHandler mouseUpHandler = nullptr;
	EventHandler eventHandler = nullptr;

	void handleKeyDown(const SDL_Keycode& code);
	void handleKeyUp(const SDL_Keycode& code);
	void handleMouseDown(const SDL_MouseButtonEvent& event);
	void handleMouseMotion(const SDL_MouseMotionEvent& event);
	void handleMouseUp(const SDL_MouseButtonEvent& event);
};
