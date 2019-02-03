#pragma once

#include <map>
#include <vector>
#include <Objects.h>

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
	const std::vector<Object*> getObjects();
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

private:
	std::vector<Object*> objects;
	std::map<const char*, int> objectMap;
	std::map<const char*, ObjLoader*> objLoaderMap;
	State state = State::ACTIVE;
};
