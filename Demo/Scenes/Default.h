#pragma once

#include <System/Scene.h>

class Default : public Scene {
public:
	void load() override;
	void onUpdate(int dt, int runningTime) override;
};
