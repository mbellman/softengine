#pragma once

#include <System/Level.h>

class Garden : public Level {
public:
	void load() override;
	void update(int dt, int runningTime) override;
};
