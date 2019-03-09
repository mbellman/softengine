#pragma once

#include <System/Scene.h>

class Garden : public Scene {
public:
	void load() override;
	void onUpdate(int dt) override;
};
