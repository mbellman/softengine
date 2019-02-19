#pragma once

#include <System/Level.h>

class Default : public Level {
public:
	void load() override;
	void onUpdate(int dt, int runningTime) override;
};
