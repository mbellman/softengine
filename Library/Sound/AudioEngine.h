#pragma once

#include <AL/al.h>
#include <AL/alut.h>

class AudioEngine {
public:
	AudioEngine();
	~AudioEngine();
};

using AudioError = ALint;
