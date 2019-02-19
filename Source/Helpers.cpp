#include <Helpers.h>
#include <random>
#include <math.h>

namespace RNG {
	namespace {
		std::default_random_engine engine;
		std::uniform_real_distribution<float> floatDistribution(0.0f, 1.0f);
	};

	float random(float low, float high) {
		return low + floor(floatDistribution(engine) * (high - low) + 1);
	}
};
