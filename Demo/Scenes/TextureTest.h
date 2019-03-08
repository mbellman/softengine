#pragma once

#include <System/Scene.h>

class TextureTest : public Scene {
public:
	void load() override;
	void onUpdate(int dt, int runningTime) override;
};
