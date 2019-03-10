#include <System/InputManager.h>
#include <Constants.h>
#include <SDL.h>

/**
 * InputManager
 * ------------
 */
void InputManager::handleEvent(const SDL_Event& event) {
	if (eventHandler != nullptr) {
		eventHandler(event);
	}

	switch (event.type) {
		case SDL_KEYDOWN:
			handleKeyDown(event.key.keysym.sym);
			break;
		case SDL_KEYUP:
			handleKeyUp(event.key.keysym.sym);
			break;
		case SDL_MOUSEMOTION:
			handleMouseMotion(event.motion);
			break;
		case SDL_MOUSEBUTTONDOWN:
			handleMouseDown(event.button);
			break;
		case SDL_MOUSEBUTTONUP:
			handleMouseUp(event.button);
			break;
	}
}

void InputManager::handleKeyDown(const SDL_Keycode& code) {
	if (keyDownHandler != nullptr) {
		keyDownHandler(code);
	}

	switch (code) {
		case SDLK_w:
			keyState |= Keys::W;
			break;
		case SDLK_a:
			keyState |= Keys::A;
			break;
		case SDLK_s:
			keyState |= Keys::S;
			break;
		case SDLK_d:
			keyState |= Keys::D;
			break;
		case SDLK_LSHIFT:
			keyState |= Keys::SHIFT;
			break;
	}
}

void InputManager::handleKeyUp(const SDL_Keycode& code) {
	if (keyUpHandler != nullptr) {
		keyUpHandler(code);
	}

	switch (code) {
		case SDLK_w:
			keyState &= ~Keys::W;
			break;
		case SDLK_s:
			keyState &= ~Keys::S;
			break;
		case SDLK_a:
			keyState &= ~Keys::A;
			break;
		case SDLK_d:
			keyState &= ~Keys::D;
			break;
		case SDLK_LSHIFT:
			keyState &= ~Keys::SHIFT;
			break;
		case SDLK_ESCAPE:
		case SDLK_SPACE:
			SDL_SetRelativeMouseMode(SDL_FALSE);
			break;
	}
}

void InputManager::handleMouseDown(const SDL_MouseButtonEvent& event) {
	switch (event.button) {
		case SDL_BUTTON_LEFT: {
			SDL_SetRelativeMouseMode(SDL_TRUE);

			lastMouseDownTime = (int)SDL_GetTicks();

			if (mouseDownHandler != nullptr) {
				mouseDownHandler();
			}

			break;
		}
	}
}

void InputManager::handleMouseMotion(const SDL_MouseMotionEvent& event) {
	if (mouseMotionHandler != nullptr && SDL_GetRelativeMouseMode()) {
		mouseMotionHandler(event.xrel, event.yrel);
	}
}

void InputManager::handleMouseUp(const SDL_MouseButtonEvent& event) {
	switch (event.button) {
		case SDL_BUTTON_LEFT: {
			if (mouseUpHandler != nullptr) {
				mouseUpHandler();
			}

			int clickTime = (int)SDL_GetTicks() - lastMouseDownTime;

			if (mouseClickHandler != nullptr && clickTime < CLICK_TIME_LIMIT) {
				mouseClickHandler();
			}

			break;
		}
	}
}

bool InputManager::isKeyPressed(Keys key) {
	return keyState & key;
}

void InputManager::onEvent(EventHandler handler) {
	eventHandler = handler;
}

void InputManager::onKeyDown(KeyHandler handler) {
	keyDownHandler = handler;
}

void InputManager::onKeyUp(KeyHandler handler) {
	keyUpHandler = handler;
}

void InputManager::onMouseClick(MouseButtonHandler handler) {
	mouseClickHandler = handler;
}

void InputManager::onMouseDown(MouseButtonHandler handler) {
	mouseDownHandler = handler;
}

void InputManager::onMouseMotion(MouseMotionHandler handler) {
	mouseMotionHandler = handler;
}

void InputManager::onMouseUp(MouseButtonHandler handler) {
	mouseUpHandler = handler;
}

void InputManager::resetKeyState() {
	keyState = 0;
}
