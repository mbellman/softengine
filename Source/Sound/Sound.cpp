#include <Sound/Sound.h>
#include <UI/Alert.h>
#include <SDL_mixer.h>
#include <stdio.h>
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
			char errorMessage[60];

			sprintf(errorMessage, "Unable to load sound: %s", filename);
			Alert::error(ALERT_ASSET_ERROR, errorMessage);
			exit(0);
		}

		Sound::mixChunkCache.emplace(filename, chunk);
	} else {
		chunk = cachedChunk->second;
	}

	alGenBuffers(1, &alAudioBuffer);
	alGenSources(1, &alAudioSource);

	alBufferData(alAudioBuffer, AL_FORMAT_MONO16, chunk->abuf, chunk->alen, 44100);
	alSourcei(alAudioSource, AL_BUFFER, alAudioBuffer);

	if(alGetError() != AL_NO_ERROR) {
		char errorMessage[60];

		sprintf(errorMessage, "Failed to generate sound buffer: %s", filename);
		Alert::error(ALERT_AUDIO_ERROR, errorMessage);
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
	for (auto& [key, mixChunk] : Sound::mixChunkCache) {
		Mix_FreeChunk(mixChunk);
	}

	Sound::mixChunkCache.clear();
}

bool Sound::isOn() {
	ALint state;

	alGetSourcei(alAudioSource, AL_SOURCE_STATE, &state);

	return state == AL_PLAYING || state == AL_LOOPING;
}

bool Sound::isPaused() {
	ALint state;

	alGetSourcei(alAudioSource, AL_SOURCE_STATE, &state);

	return state == AL_PAUSED;
}

void Sound::loop() {
	alSourcei(alAudioSource, AL_LOOPING, AL_TRUE);
	alSourcePlay(alAudioSource);

	playMode = PlayMode::LOOPING;
}

void Sound::pause() {
	if (isOn()) {
		alSourcePause(alAudioSource);
	}
}

void Sound::play() {
	alSourcei(alAudioSource, AL_LOOPING, AL_FALSE);
	alSourcePlay(alAudioSource);

	playMode = PlayMode::PLAYING;
}

void Sound::resume() {
	if (isPaused()) {
		if (playMode == PlayMode::PLAYING) {
			play();
		} else if (playMode == PlayMode::LOOPING) {
			loop();
		}
	}
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
