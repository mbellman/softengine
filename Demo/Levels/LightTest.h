#pragma once

#include <System/Level.h>

class LightTest : public Level {
public:
	void load() override;
	void onUpdate(int dt, int runningTime) override;
};
