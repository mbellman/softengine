#pragma once

#include <System/Level.h>

class Beach : public Level {
public:
	void load() override;
	void onUpdate(int dt, int runningTime) override;
};
