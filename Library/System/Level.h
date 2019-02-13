#pragma once

#include <map>
#include <vector>
#include <limits.h>
#include <System/Objects.h>
#include <Graphics/TextureBuffer.h>

enum State {
	ACTIVE,
	INACTIVE
};

struct Settings {
	Color backgroundColor = { 0, 0, 0 };
	Color ambientLightColor = { 0, 0, 0 };
	Vec3 ambientLightVector = { 0, -1, 0 };
	float ambientLightFactor = 1.0f;
	float brightness = 1.0f;
	int visibility = INT_MAX;
};

/**
 * Level
 * -----
 */
class Level {
public:
	const std::vector<Object*>& getObjects();
	const std::vector<Light*>& getLights();
	const Settings& getSettings();
	bool hasQuit();
	virtual void load() = 0;
	void quit();
	virtual void update(int dt, int runningTime);

protected:
	Settings settings;

	void add(Object* object);
	void add(const char* key, Object* object);
	void add(const char* key, ObjLoader* objLoader);
	void add(const char* key, TextureBuffer* textureBuffer);
	Object* getObject(const char* key);
	ObjLoader* getObjLoader(const char* key);
	TextureBuffer* getTexture(const char* key);
	void remove(const char* key);

private:
	std::vector<Object*> objects;
	std::vector<Light*> lights;
	std::map<const char*, int> objectMap;
	std::map<const char*, ObjLoader*> objLoaderMap;
	std::map<const char*, TextureBuffer*> textureBufferMap;
	State state = State::ACTIVE;

	template<class T>
	T* getMapItem(std::map<const char*, T*> map, const char* key);

	void removeLight(Light* light);

	template<class T>
	void removeMapItem(std::map<const char*, T*> map, const char* key);
};
