#include <UI/UI.h>

UI::~UI() {
	for (auto& [key, uiObject] : uiObjectMap) {
		delete uiObject;
	}

	uiObjectMap.clear();
}

void UI::update(int dt) {
	for(auto& [key, uiObject] : uiObjectMap) {
		uiObject->update(dt);
	}
}

void UI::add(const char* key, UIObject* uiObject) {
	uiObjectMap.emplace(key, uiObject);
}

UIObject* UI::get(const char* key) {
	const auto& uiObject = uiObjectMap.find(key);

	if (uiObject != uiObjectMap.end()) {
		return uiObject->second;
	}

	return nullptr;
}
