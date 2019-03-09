#pragma once

#include <System/Scene.h>

class LightTest : public Scene {
public:
	void load() override;
	void onUpdate(int dt) override;
};
