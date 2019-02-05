#include <string>
#include <vector>
#include <algorithm>
#include <Level.h>
#include <Objects.h>

/**
 * Level
 * -----
 */
void Level::add(const char* key, Object* object) {
	add(object);

	objectMap.emplace(key, objects.size() - 1);
}

void Level::add(Object* object) {
	objects.push_back(object);

	if (Light::isLight(object)) {
		lights.push_back((Light*)object);
	}
}

void Level::add(const char* key, ObjLoader* objLoader) {
	objLoaderMap.emplace(key, objLoader);
}

Object* Level::getObject(const char* key) {
	auto it = objectMap.find(key);

	if (it != objectMap.end()) {
		return objects.at(it->second);
	}

	return NULL;
}

const std::vector<Object*>& Level::getObjects() {
	return objects;
}

const std::vector<Light*>& Level::getLights() {
	return lights;
}

const Settings& Level::getSettings() {
	return settings;
}

bool Level::hasQuit() {
	return state == State::INACTIVE;
}

void Level::quit() {
	for (auto& object : objects) {
		delete object;
	}

	for (auto& [key, objLoader] : objLoaderMap) {
		delete objLoader;
	}

	objects.clear();
	lights.clear();
	objectMap.clear();
	objLoaderMap.clear();

	state = State::INACTIVE;
}

void Level::remove(const char* key) {
	auto it = objectMap.find(key);

	if (it != objectMap.end()) {
		Object* object = objects.at(it->second);

		if (Light::isLight(object)) {
			removeLight((Light*)object);
		}

		delete object;

		objects.erase(objects.begin() + it->second);
		objectMap.erase(key);
	}
}

void Level::removeLight(Light* light) {
	int index = 0;

	while (index < lights.size()) {
		if(lights.at(index) == light) {
			lights.erase(lights.begin() + index);
		} else {
			index++;
		}
	}
}

void Level::update(int dt, int runningTime) {}
