#include <Sound/Sound.h>
#include <Sound/AudioEngine.h>
#include <iostream>
#include <SDL_mixer.h>
#include <al.h>
#include <map>

/**
 * Sound
 * -----
 */
Sound::Sound(const char* filename) {
	Mix_Chunk* chunk;
	auto cachedChunk = Sound::mixChunkCache.find(filename);

	if (cachedChunk == mixChunkCache.end()) {
		chunk = Mix_LoadWAV(filename);

		if (!chunk) {
			std::cout << "Unable to load sound: " << filename << std::endl;

			exit(0);
		}

		mixChunkCache.emplace(filename, chunk);
	} else {
		chunk = cachedChunk->second;
	}

	alGenBuffers(1, &alAudioBuffer);
	alGenSources(1, &alAudioSource);

	alBufferData(alAudioBuffer, AL_FORMAT_MONO16, chunk->abuf, chunk->alen, 44100);
	alSourcei(alAudioSource, AL_BUFFER, alAudioBuffer);

	if(alGetError() != AL_NO_ERROR) {
		std::cout << "Failed to generate sound buffer: " << filename << std::endl;

		exit(0);
	}
}

Sound::~Sound() {
	stop();

	alDeleteSources(1, &alAudioSource);
    alDeleteBuffers(1, &alAudioBuffer);
}

std::map<const char*, Mix_Chunk*> Sound::mixChunkCache;

void Sound::clearMixChunkCache() {
	for (auto& [key, mixChunk] : mixChunkCache) {
		delete mixChunk;
	}

	Sound::mixChunkCache.clear();
}

void Sound::loop() {
	alSourcei(alAudioSource, AL_LOOPING, AL_TRUE);
	alSourcePlay(alAudioSource);
}

void Sound::play() {
	alSourcei(alAudioSource, AL_LOOPING, AL_FALSE);
	alSourcePlay(alAudioSource);
}

void Sound::setApparentPosition(const Vec3& apparentPosition) {
	alSource3f(alAudioSource, AL_POSITION, apparentPosition.x, apparentPosition.y, apparentPosition.z);
}

void Sound::setVolume(float volume) {
	alSourcef(alAudioSource, AL_GAIN, volume);
}

void Sound::stop() {
	alSourceStop(alAudioSource);
}
