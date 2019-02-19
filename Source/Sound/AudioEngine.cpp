#include <iostream>
#include <cstddef>

#include <AL/alut.h>

#include <Sound/AudioEngine.h>

AudioEngine::AudioEngine() {
	alutInit(0, NULL);
	// Clear error codes
	alGetError();
}

AudioEngine::~AudioEngine() {
	alutExit();
}
