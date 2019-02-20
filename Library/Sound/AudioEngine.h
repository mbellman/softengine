#pragma once

#include <SDL_mixer.h>
#include <al.h>
#include <alc.h>

class AudioEngine {
public:
	AudioEngine();
	~AudioEngine();

private:
	ALCdevice* device = nullptr;
	ALCcontext* context = nullptr;
};
