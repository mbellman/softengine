#pragma once

#include <Level.h>

class Default : public Level {
public:
	void load() override;
	void update(int dt, int runningTime) override;
};
