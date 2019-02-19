#include <iostream>
#include <cstddef>

#include <AL/alut.h>

#include <Sound/AudioEngine.h>



AudioEngine::AudioEngine() {
	alutInit(0, NULL);
	alGetError();
}

AudioEngine::~AudioEngine() {
	alutExit();
}
