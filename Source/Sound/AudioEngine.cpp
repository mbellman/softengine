#include <Sound/AudioEngine.h>
#include <SDL_mixer.h>
#include <al.h>
#include <alc.h>
#include <iostream>
#include <UI/Alert.h>

using namespace std;

/**
 * AudioEngine
 * -----------
 */
AudioEngine::AudioEngine() {
	// Initialize SDL_mixer
	Mix_Init(0);

	if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) != 0) {
		Alert::error(ALERT_ERROR, "Error initializing SDL mixer audio");
		exit(0);
	}

	// Initialize OpenAL
	device = alcOpenDevice(0);

	if (device) {
		context = alcCreateContext(device, 0);
		alcMakeContextCurrent(context);
	} else {
		Alert::error(ALERT_ERROR, "Error initializing audio device");
		exit(0);
	}
}

AudioEngine::~AudioEngine() {
	alcCloseDevice(device);
	alcDestroyContext(context);

	Mix_CloseAudio();
	Mix_Quit();
}

void AudioEngine::mute() {
	alListenerf(AL_GAIN, 0.0f);
}

void AudioEngine::unmute() {
	alListenerf(AL_GAIN, 1.0f);
}
