#pragma once

#include <al.h>
#include <alc.h>
#include <System/Geometry.h>
#include <SDL_mixer.h>
#include <map>

class Sound {
public:
	Vec3 position;

	Sound(const char* filename);
	~Sound();

	static void clearMixChunkCache();

	void loop();
	void play();
	void setApparentPosition(const Vec3& apparentPosition);
	void setVolume(float volume);
	void stop();

private:
	static std::map<const char*, Mix_Chunk*> mixChunkCache;

	ALuint alAudioBuffer;
	ALuint alAudioSource;
};
