#pragma once

#include <vector>
#include <SDL.h>

#include <UI/UIObjects.h>

class UI {
public:
	~UI();

	void render();
	void addObject(UIObject* object);

protected:
	std::vector<UIObject*> m_objects;
};
