#include <iostream>

#include <Sound/AudioEngine.h>
#include <Sound/Sound.h>

Sound::Sound(const char* filename) {
	m_buffer = alutCreateBufferFromFile(filename);

	alGenSources(1, &m_source);

	alSourcei(m_source, AL_BUFFER, m_buffer);

	if(loop) alSourcei(m_source, AL_LOOPING, 1);
}

void Sound::play() {
	std::cout << "I was played\n";
	ALint error;
	alSourcePlay(m_source);
	if((error = alGetError()) != AL_NO_ERROR) {
		std::cout << "Failed to play sound: " << alutGetErrorString(error) << std::endl;
	}
}

bool Sound::isPlaying() {
	alGetSourcei(m_source, AL_SOURCE_STATE, &m_state);
	return m_state == AL_PLAYING || AL_LOOPING;
}

void Sound::setPosition(const Vec3& position) {
	m_position = position;
	alSource3f(m_source, AL_POSITION, position.x, position.y, position.z);
}

void Sound::setVelocity(const Vec3& velocity) {
	m_velocity = velocity;
	alSource3f(m_source, AL_POSITION, velocity.x, velocity.y, velocity.z);
}

Sound::~Sound() {
	alDeleteSources(1, &m_source);
    alDeleteBuffers(1, &m_buffer);
}
