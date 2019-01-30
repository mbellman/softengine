#include <UI/UI.h>

UI::~UI() {
	m_objects.clear();
}

void UI::draw() {
	for(auto &object : m_objects) {
		object->draw();
	}
}

void UI::addObject(UIObject* object) {
	m_objects.push_back(object);
}
