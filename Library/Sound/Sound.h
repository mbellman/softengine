#pragma once

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>
#include <System/Geometry.h>

class Sound {
public:
	Sound(const char* filename);
	~Sound();

	Vec3 position = {0, 0, 0};
	Vec3 velocity = {0, 0, 0};
	float volume = 0.75;
	bool loop = false;

	void play();
	bool isPlaying();

protected:
	ALuint m_buffer, m_source;
	ALint m_state;
};
