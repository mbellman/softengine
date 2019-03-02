#include <System/Level.h>
#include <string>
#include <vector>
#include <algorithm>
#include <functional>
#include <cmath>
#include <System/Objects.h>
#include <System/InputManager.h>
#include <Graphics/TextureBuffer.h>
#include <Sound/Sound.h>

/**
 * Level
 * -----
 */
Level::Level() {
	inputManager = new InputManager();

	inputManager->onMouseMotion([=](int dx, int dy) {
		handleMouseMotion(dx, dy);
	});
}

Level::~Level() {
	if (state != LevelState::INACTIVE) {
		quit();
	}
}

void Level::add(const char* key, Object* object) {
	add(object);

	objectMap.emplace(key, objects.size() - 1);
}

void Level::add(Object* object) {
	object->syncLODs();
	object->computeSurfaceNormals();

	objects.push_back(object);

	if (object->isOfType<Light>()) {
		lights.push_back((Light*)object);
	}
}

void Level::add(Sound* sound) {
	sounds.push_back(sound);
}

void Level::add(const char* key, ObjLoader* objLoader) {
	objLoaderMap.emplace(key, objLoader);
}

void Level::add(const char* key, TextureBuffer* textureBuffer) {
	textureBufferMap.emplace(key, textureBuffer);
}

void Level::add(const char* key, Sound* sound) {
	soundMap.emplace(key, sound);

	add(sound);
}

void Level::addParticleSystem(const char* key, ParticleSystem* particleSystem) {
	particleSystemMap.emplace(key, particleSystem);

	for (auto* particle : particleSystem->getParticles()) {
		objects.push_back(particle);
	}
}

Object* Level::getObject(const char* key) {
	auto it = objectMap.find(key);

	if (it != objectMap.end()) {
		return objects.at(it->second);
	}

	return NULL;
}

Sound* Level::getSound(const char* key) {
	return getMapItem(soundMap, key);
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

const std::vector<Sound*>& Level::getSounds() {
	return sounds;
}

const Settings& Level::getSettings() {
	return settings;
}

void Level::handleControl(int dt) {
	if (settings.controlMode & ControlMode::WASD) {
		handleWASDControl(dt);
	}
}

void Level::handleMouseMotion(int dx, int dy) {
	if (~settings.controlMode & ControlMode::MOUSE) {
		return;
	}

	float deltaFactor = 1.0f / 500.0f;
	float yawDelta = (float)-dx * deltaFactor;
	float pitchDelta = (float)-dy * deltaFactor;

	camera->yaw += yawDelta;
	camera->pitch = std::clamp(camera->pitch + pitchDelta, -Camera::MAX_PITCH, Camera::MAX_PITCH);
}

void Level::handleWASDControl(int dt) {
	Vec3 velocity;
	float movementDelta = Level::MOVEMENT_SPEED * (dt / 16.0f);

	if (inputManager->isKeyPressed(Keys::W)) {
		velocity.z = movementDelta;
	} else if (inputManager->isKeyPressed(Keys::S)) {
		velocity.z = -movementDelta;
	}

	if (inputManager->isKeyPressed(Keys::A)) {
		velocity.x = -movementDelta;
	} else if (inputManager->isKeyPressed(Keys::D)) {
		velocity.x = movementDelta;
	}

	if (inputManager->isKeyPressed(Keys::SHIFT)) {
		velocity *= 4;
	}

	float sy = sinf(camera->yaw);
	float cy = cosf(camera->yaw);

	camera->position.x += cy * velocity.x + sy * -velocity.z;
	camera->position.z += cy * velocity.z + sy * velocity.x;
}

bool Level::hasQuit() {
	return state == LevelState::INACTIVE;
}

void Level::onStart() {}
void Level::onUpdate(int dt, int runningTime) {}

void Level::quit() {
	for (auto* object : objects) {
		if (!object->isOfType<Particle>()) {
			// Only delete Objects other than Particles, which
			// are managed by their source ParticleSystems
			delete object;
		}
	}

	for (auto* sound : sounds) {
		delete sound;
	}

	for (auto& [key, objLoader] : objLoaderMap) {
		delete objLoader;
	}

	for (auto& [key, textureBuffer] : textureBufferMap) {
		delete textureBuffer;
	}

	for (auto& [key, particleSystem] : particleSystemMap) {
		delete particleSystem;
	}

	delete inputManager;

	objects.clear();
	lights.clear();
	sounds.clear();
	objectMap.clear();
	objLoaderMap.clear();
	textureBufferMap.clear();
	particleSystemMap.clear();

	Sound::clearMixChunkCache();

	state = LevelState::INACTIVE;
}

void Level::remove(const char* key) {
	safelyRemoveKeyedObject(key);
	safelyRemoveKeyedParticleSystem(key);

	removeMapItem(objLoaderMap, key);
	removeMapItem(textureBufferMap, key);
	removeMapItem(particleSystemMap, key);
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
	auto entry = map.find(key);

	if (entry != map.end()) {
		delete map.at(key);

		map.erase(key);

		return;
	}
}

/**
 * Removes a mapped Object if the provided key matches an entry.
 * The Object must be deleted in the following fashion:
 *
 *  A) From the map
 *  B) From the Object pointer list
 *  C) If it is a Light, from the Lights pointer list
 */
void Level::safelyRemoveKeyedObject(const char* key) {
	auto entry = objectMap.find(key);

	if (entry != objectMap.end()) {
		int idx = entry->second;

		Object* object = objects.at(idx);

		if (object->isOfType<Light>()) {
			removeLight((Light*)object);
		}

		delete object;

		objects.erase(objects.begin() + idx);
		objectMap.erase(key);
	}
}

/**
 * Removes a mapped ParticleSystem if the provided key matches
 * an entry. When a ParticleSystem is removed, all of its Particle
 * Objects must be removed from the Object pointer list, and the
 * ParticleSystem itself deleted, thus freeing the allocated
 * Particles. Since all of a ParticleSystem's Particles are stored
 * contiguously in the Object pointer list, we can erase all list
 * elements from [N = first particle index, N + particle system size).
 */
void Level::safelyRemoveKeyedParticleSystem(const char* key) {
	auto entry = particleSystemMap.find(key);

	if (entry != particleSystemMap.end()) {
		ParticleSystem* particleSystem = particleSystemMap.at(key);
		const std::vector<Particle*>& particles = particleSystem->getParticles();
		Particle* firstParticle = particles.at(0);
		int idx = 0;

		while (idx < objects.size()) {
			if (objects.at(idx) == firstParticle) {
				for (int n = 0; n < particles.size(); n++) {
					objects.erase(objects.begin() + idx);
				}

				break;
			}

			idx++;
		}

		delete particleSystem;

		particleSystemMap.erase(key);
	}
}

void Level::setCamera(Camera* camera) {
	this->camera = camera;
}

void Level::update(int dt) {
	for (auto* object : objects) {
		object->update(dt);
	}

	for (auto [key, particleSystem] : particleSystemMap) {
		particleSystem->update(dt);
	}

	handleControl(dt);
	camera->update();
}
