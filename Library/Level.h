#pragma once

#include <map>
#include <vector>
#include <Objects.h>
#include <Types.h>

enum State {
	ACTIVE,
	INACTIVE
};

/**
 * Level
 * -----
 */
class Level {
public:
	const Color& getBackgroundColor();
	const std::vector<Object*>& getObjects();
	const std::vector<Light*>& getLights();
	bool hasQuit();
	virtual void load() = 0;
	void quit();
	virtual void update(int dt, int runningTime);

protected:
	void add(const char* key, Object* object);
	void add(Object* object);
	void add(const char* key, ObjLoader* objLoader);
	Object* getObject(const char* key);
	ObjLoader* getLoader(const char* key);
	void remove(const char* key);
	void setBackgroundColor(int R, int G, int B);

private:
	Color backgroundColor = { 0, 0, 0 };
	std::vector<Object*> objects;
	std::vector<Light*> lights;
	std::map<const char*, int> objectMap;
	std::map<const char*, ObjLoader*> objLoaderMap;
	State state = State::ACTIVE;

	void removeLight(Light* light);
};
