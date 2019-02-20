#include <Sound/Sound.h>
#include <Sound/AudioEngine.h>
#include <iostream>
#include <SDL_mixer.h>
#include <al.h>

Sound::Sound(const char* filename) {
	this->filename = filename;

	Mix_Chunk* chunk = Mix_LoadWAV(filename);

	if (!chunk) {
		std::cout << "Unable to load sound: " << filename << std::endl;

		return;
	}

	alGenBuffers(1, &m_buffer);
	alGenSources(1, &m_source);

	alBufferData(m_buffer, AL_FORMAT_MONO16, chunk->abuf, chunk->alen, 44100);
	alSourcei(m_source, AL_BUFFER, m_buffer);
	alSourcef(m_source, AL_PITCH, 1.0f);
	alSourcef(m_source, AL_GAIN, 1.0f);

	ALint error;

	if((error = alGetError()) != AL_NO_ERROR) {
		std::cout << "Failed to generate sound buffer: " << filename << std::endl;

		return;
	}

	Mix_FreeChunk(chunk);

	isInitialized = true;
}

void Sound::loops(bool shouldLoop) {
	if (!isInitialized) {
		return;
	}

	m_loop = shouldLoop;

	alSourcei(m_source, AL_LOOPING, shouldLoop ? AL_TRUE : AL_FALSE);
}

void Sound::play() {
	if (!isInitialized) {
		return;
	}

	ALint error;

	alSourcePlay(m_source);

	if((error = alGetError()) != AL_NO_ERROR) {
		std::cout << "Failed to play sound: " << filename << std::endl;
	}
}

bool Sound::isPlaying() {
	if (!isInitialized) {
		return false;
	}

	alGetSourcei(m_source, AL_SOURCE_STATE, &m_state);

	return m_state == AL_PLAYING || AL_LOOPING;
}

void Sound::setApparentPosition(const Vec3& apparentPosition) {
	if (!isInitialized) {
		return;
	}

	alSource3f(m_source, AL_POSITION, apparentPosition.x, apparentPosition.y, apparentPosition.z);
}

Sound::~Sound() {
	if (!isInitialized) {
		return;
	}

	alDeleteSources(1, &m_source);
    alDeleteBuffers(1, &m_buffer);
}
