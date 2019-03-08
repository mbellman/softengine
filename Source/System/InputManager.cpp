#include <System/InputManager.h>
#include <SDL.h>

/**
 * InputManager
 * ------------
 */
void InputManager::handleEvent(const SDL_Event& event) {
	switch (event.type) {
		case SDL_KEYDOWN:
			handleKeyDown(event.key.keysym.sym);
			break;
		case SDL_KEYUP:
			handleKeyUp(event.key.keysym.sym);
			break;
		case SDL_MOUSEMOTION:
			handleMouseMotionEvent(event.motion);
			break;
		case SDL_MOUSEBUTTONDOWN:
			if (event.button.button == SDL_BUTTON_LEFT) {
				SDL_SetRelativeMouseMode(SDL_TRUE);
			}
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

void InputManager::handleMouseMotionEvent(const SDL_MouseMotionEvent& event) {
	if (mouseMotionHandler && SDL_GetRelativeMouseMode()) {
		mouseMotionHandler(event.xrel, event.yrel);
	}
}

bool InputManager::isKeyPressed(Keys key) {
	return keyState & key;
}

void InputManager::onKeyDown(KeyHandler handler) {
	keyDownHandler = handler;
}

void InputManager::onKeyUp(KeyHandler handler) {
	keyUpHandler = handler;
}

void InputManager::onMouseMotion(MouseMotionHandler handler) {
	mouseMotionHandler = handler;
}

void InputManager::resetKeyState() {
	keyState = 0;
}
