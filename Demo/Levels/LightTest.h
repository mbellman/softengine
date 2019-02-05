#pragma once

#include <Level.h>

class LightTest : public Level {
public:
	void load() override;
	void update(int dt, int runningTime) override;
};
