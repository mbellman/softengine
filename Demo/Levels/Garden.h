#pragma once

#include <System/Level.h>

class Garden : public Level {
public:
	void load() override;
	void onUpdate(int dt, int runningTime) override;
};
