#include <UI/UI.h>
#include <algorithm>
#include <SDL.h>

/**
 * UI
 * --
 */
UI::UI(SDL_Renderer* renderer) {
	this->renderer = renderer;
}

UI::~UI() {
	for (auto* uiObject : uiObjects) {
		delete uiObject;
	}

	uiObjectMap.clear();
	uiObjects.clear();
}

void UI::add(const char* key, UIObject* uiObject) {
	uiObject->setRenderer(renderer);

	uiObjects.push_back(uiObject);
	uiObjectMap.emplace(key, uiObject);
}

UIObject* UI::get(const char* key) {
	const auto& uiObject = uiObjectMap.find(key);

	if (uiObject != uiObjectMap.end()) {
		return uiObject->second;
	}

	return nullptr;
}

void UI::remove(const char* key) {
	const auto& entry = uiObjectMap.find(key);

	if (entry != uiObjectMap.end()) {
		UIObject* uiObject = entry->second;

		uiObjectMap.erase(key);
		uiObjects.erase(std::remove(uiObjects.begin(), uiObjects.end(), uiObject), uiObjects.end());

		delete uiObject;
	}
}

void UI::update(int dt) {
	for(auto* uiObject : uiObjects) {
		uiObject->update(dt);
	}
}
