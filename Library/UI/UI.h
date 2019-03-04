#pragma once

#include <map>
#include <SDL.h>
#include <UI/UIObjects.h>

/**
 * UI
 * --
 */
class UI {
public:
	~UI();

	void add(const char* key, UIObject* object);
	UIObject* get(const char* key);
	void update(int dt);

protected:
	std::map<const char*, UIObject*> uiObjectMap;
};
