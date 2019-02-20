#pragma once

#include <al.h>
#include <alc.h>
#include <System/Geometry.h>
#include <SDL_mixer.h>

class Sound {
public:
	float volume = 0.75f;
	Vec3 position;

	Sound(const char* filename);
	~Sound();

	bool isPlaying();
	void loops(bool value);
	void play();
	void setApparentPosition(const Vec3& apparentPosition);

private:
	const char* filename;
	bool isInitialized = false;
	bool m_loop = false;
	ALuint m_buffer, m_source;
	ALint m_state;
};
