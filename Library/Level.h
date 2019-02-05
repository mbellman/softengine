#pragma once

#include <map>
#include <vector>
#include <limits.h>
#include <Objects.h>
#include <Types.h>

enum State {
	ACTIVE,
	INACTIVE
};

struct Settings {
	Color backgroundColor = { 0, 0, 0 };
	Color ambientLightColor = { 0, 0, 0 };
	Vec3 ambientLightVector = { 0, -1, 0 };
	float ambientLightFactor = 0;
	float albedo = 1;
	float drawDistance = INT_MAX;
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

	void add(const char* key, Object* object);
	void add(Object* object);
	void add(const char* key, ObjLoader* objLoader);
	Object* getObject(const char* key);
	ObjLoader* getLoader(const char* key);
	void remove(const char* key);

private:
	std::vector<Object*> objects;
	std::vector<Light*> lights;
	std::map<const char*, int> objectMap;
	std::map<const char*, ObjLoader*> objLoaderMap;
	State state = State::ACTIVE;

	void removeLight(Light* light);
};
