#pragma once

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>
#include <System/Geometry.h>

class Sound {
public:
	Sound(const char* filename);
	~Sound();

	float volume = 0.75;
	bool loop = false;

	void play();
	bool isPlaying();

	void setPosition(const Vec3& position);
	void setVelocity(const Vec3& velocity);

protected:
	ALuint m_buffer, m_source;
	ALint m_state;
	Vec3 m_position = {0, 0, 0};
	Vec3 m_velocity = {0, 0, 0};
};
