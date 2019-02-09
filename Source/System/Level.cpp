#include <System/Level.h>
#include <string>
#include <vector>
#include <algorithm>
#include <System/Objects.h>
#include <Graphics/TextureBuffer.h>

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

void Level::add(const char* key, TextureBuffer* textureBuffer) {
	textureBufferMap.emplace(key, textureBuffer);
}

Object* Level::getObject(const char* key) {
	auto it = objectMap.find(key);

	if (it != objectMap.end()) {
		return objects.at(it->second);
	}

	return NULL;
}

ObjLoader* Level::getObjLoader(const char* key) {
	return getMapItem(objLoaderMap, key);
}

TextureBuffer* Level::getTexture(const char* key) {
	return getMapItem(textureBufferMap, key);
}

template<class T>
T* Level::getMapItem(std::map<const char*, T*> map, const char* key) {
	try {
		return map.at(key);
	} catch (const std::out_of_range& error) {
		printf("Level: Could not find item '%s'\n", key);
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

	for (auto& [key, textureBuffer] : textureBufferMap) {
		delete textureBuffer;
	}

	objects.clear();
	lights.clear();
	objectMap.clear();
	objLoaderMap.clear();
	textureBufferMap.clear();

	state = State::INACTIVE;
}

void Level::remove(const char* key) {
	auto it = objectMap.find(key);

	// If the key corresponds to an Object, we have to
	// ensure that it's destroyed properly, since objects
	// are stored differently than loaders or textures.
	if (it != objectMap.end()) {
		Object* object = objects.at(it->second);

		if (Light::isLight(object)) {
			removeLight((Light*)object);
		}

		delete object;

		objects.erase(objects.begin() + it->second);
		objectMap.erase(key);
	}

	// Ensure that the key is removed from any additional
	// maps, and its value freed from memory
	removeMapItem(objLoaderMap, key);
	removeMapItem(textureBufferMap, key);
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

template<class T>
void Level::removeMapItem(std::map<const char*, T*> map, const char* key) {
	auto it = map.find(key);

	if (it != map.end()) {
		delete map.at(key);

		map.erase(key);

		return;
	}
}

void Level::update(int dt, int runningTime) {}
