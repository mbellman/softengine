#pragma once

#include <System/Level.h>

class TextureTest : public Level {
public:
	void load() override;
	void onUpdate(int dt, int runningTime) override;
};
