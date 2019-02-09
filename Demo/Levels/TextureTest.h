#pragma once

#include <System/Level.h>

class TextureTest : public Level {
public:
	void load() override;
	void update(int dt, int runningTime) override;
};
