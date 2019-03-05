#include <UI/UI.h>
#include <SDL.h>

/**
 * UI
 * --
 */
UI::UI(SDL_Renderer* renderer) {
	this->renderer = renderer;
}

UI::~UI() {
	for (auto& [key, uiObject] : uiObjectMap) {
		delete uiObject;
	}

	uiObjectMap.clear();
}

void UI::add(const char* key, UIObject* uiObject) {
	uiObject->setRenderer(renderer);
	uiObjectMap.emplace(key, uiObject);
}

UIObject* UI::get(const char* key) {
	const auto& uiObject = uiObjectMap.find(key);

	if (uiObject != uiObjectMap.end()) {
		return uiObject->second;
	}

	return nullptr;
}

void UI::update(int dt) {
	for(auto& [key, uiObject] : uiObjectMap) {
		uiObject->update(dt);
	}
}
