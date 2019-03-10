#pragma once

#include <map>
#include <vector>
#include <SDL.h>
#include <UI/UIObjects.h>

/**
 * UI
 * --
 */
class UI {
public:
	UI(SDL_Renderer* renderer);
	~UI();

	void add(const char* key, UIObject* object);
	UIObject* get(const char* key);
	void remove(const char* key);
	void update(int dt);

protected:
	SDL_Renderer* renderer = NULL;
	std::vector<UIObject*> uiObjects;
	std::map<const char*, UIObject*> uiObjectMap;
};
