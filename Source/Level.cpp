#include <string>
#include <vector>
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
}

void Level::add(const char* key, ObjLoader* objLoader) {
	objLoaderMap.emplace(key, objLoader);
}

const Color& Level::getBackgroundColor() {
	return backgroundColor;
}

Object* Level::getObject(const char* key) {
	auto it = objectMap.find(key);

	if (it != objectMap.end()) {
		return objects.at(it->second);
	}

	return NULL;
}

const std::vector<Object*> Level::getObjects() {
	return objects;
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
	objectMap.clear();
	objLoaderMap.clear();

	state = State::INACTIVE;
}

void Level::remove(const char* key) {
	auto it = objectMap.find(key);

	if (it != objectMap.end()) {
		delete objects.at(it->second);

		objects.erase(objects.begin() + it->second);
		objectMap.erase(key);
	}
}

void Level::setBackgroundColor(int R, int G, int B) {
	backgroundColor.R = R;
	backgroundColor.G = G;
	backgroundColor.B = B;
}

void Level::update(int dt, int runningTime) {}
