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
	std::cout << "Initializing AudioEngine...\n";
	alutInit(0, NULL);
	alGetError();

	ALfloat listenerPos[]={0.0,0.0,4.0};
	ALfloat listenerVel[]={0.0,0.0,0.0};
	ALfloat listenerOri[]={0.0,0.0,1.0, 0.0,1.0,0.0};
}
