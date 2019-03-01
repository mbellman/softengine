#pragma once

#include <map>
#include <vector>
#include <climits>
#include <functional>

#include <Sound/Sound.h>
#include <System/Objects.h>
#include <System/ParticleSystem.h>
#include <System/Camera.h>
#include <System/InputManager.h>
#include <Graphics/TextureBuffer.h>
#include <SDL.h>

/**
 * LevelState
 * ----------
 */
enum LevelState {
	ACTIVE,
	INACTIVE
};

/**
 * ControlMode
 * -----------
 */
enum ControlMode {
	DISABLED = 1 << 0,
	WASD = 1 << 1,
	MOUSE = 1 << 2
};

/**
 * Settings
 * --------
 */
struct Settings {
	Color backgroundColor = { 0, 0, 0 };
	Color ambientLightColor = { 0, 0, 0 };
	Vec3 ambientLightVector = { 0, -1, 0 };
	float ambientLightFactor = 1.0f;
	bool hasStaticAmbientLight = false;
	float brightness = 1.0f;
	int visibility = INT_MAX;
	int controlMode = ControlMode::WASD | ControlMode::MOUSE;
};

/**
 * Level
 * -----
 */
class Level {
public:
	InputManager* inputManager = NULL;

	Level();
	~Level();

	const std::vector<Object*>& getObjects();
	const std::vector<Light*>& getLights();
	const std::vector<Sound*>& getSounds();
	const Settings& getSettings();
	bool hasQuit();
	virtual void load() = 0;
	virtual void onStart();
	virtual void onUpdate(int dt, int runningTime);
	void quit();
	void setCamera(Camera* camera);
	void update(int dt);

protected:
	Camera* camera = NULL;
	Settings settings;

	void add(Object* object);
	void add(Sound* sound);
	void add(const char* key, Object* object);
	void add(const char* key, ObjLoader* objLoader);
	void add(const char* key, TextureBuffer* textureBuffer);
	void add(const char* key, Sound* sound);
	void addParticleSystem(const char* key, ParticleSystem* particleSystem);
	Object* getObject(const char* key);
	ObjLoader* getObjLoader(const char* key);
	TextureBuffer* getTexture(const char* key);
	Sound* getSound(const char* key);
	void remove(const char* key);

private:
	constexpr static float MOVEMENT_SPEED = 5.0f;

	std::vector<Object*> objects;
	std::vector<Light*> lights;
	std::vector<Sound*> sounds;
	std::map<const char*, int> objectMap;
	std::map<const char*, ObjLoader*> objLoaderMap;
	std::map<const char*, TextureBuffer*> textureBufferMap;
	std::map<const char*, ParticleSystem*> particleSystemMap;
	std::map<const char*, Sound*> soundMap;
	LevelState state = LevelState::ACTIVE;

	template<class T>
	T* getMapItem(std::map<const char*, T*> map, const char* key);
	void handleControl();
	void handleWASDControl();
	void handleMouseMotion(int dx, int dy);
	void removeLight(Light* light);
	template<class T>
	void removeMapItem(std::map<const char*, T*> map, const char* key);
	void safelyRemoveKeyedObject(const char* key);
	void safelyRemoveKeyedParticleSystem(const char* key);
};
