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
	void pause();
	void play();
	void resume();
	void setApparentPosition(const Vec3& apparentPosition);
	void setVolume(float volume);
	void stop();

private:
	/**
	 * PlayMode
	 * --------
	 *
	 * Provides state constants which can help
	 * manage sound resuming behavior.
	 */
	enum PlayMode {
		OFF = 0,
		PLAYING = 1,
		LOOPING = 2
	};

	static std::map<const char*, Mix_Chunk*> mixChunkCache;
	ALuint alAudioBuffer;
	ALuint alAudioSource;
	PlayMode playMode = PlayMode::OFF;

	bool isOn();
	bool isPaused();
};
