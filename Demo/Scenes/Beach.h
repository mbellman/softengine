#pragma once

#include <System/Scene.h>

class Beach : public Scene {
public:
	void load() override;
	void onUpdate(int dt) override;
};
