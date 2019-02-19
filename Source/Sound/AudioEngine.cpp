#include <iostream>
#include <cstddef>

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>

#include <Sound/AudioEngine.h>

#define NUM_BUFFERS 1
#define NUM_SOURCES 1
#define NUM_ENVIRONMENTS 1

bool AudioEngine::init() {
	ALuint buffer, source;
    ALint state;

	alutInit(0, NULL);
	alGetError();

	buffer = alutCreateBufferFromFile("./DemoAssets/applause.wav");

	alGenSources(1, &source);
    alSourcei(source, AL_BUFFER, buffer);

	alSourcePlay(source);

    // Wait for the song to complete
    do {
        alGetSourcei(source, AL_SOURCE_STATE, &state);
    } while (state == AL_PLAYING);

    // Clean up sources and buffers
    alDeleteSources(1, &source);
    alDeleteBuffers(1, &buffer);

    // Exit everything
    alutExit();
}
