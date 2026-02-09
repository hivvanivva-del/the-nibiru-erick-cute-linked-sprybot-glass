#pragma once
#include "../../ModuleBase/Module.h"

class AutoWalk : public Module {
private:
	float speed = 1.f;
public:
	AutoWalk();

	void onNormalTick(LocalPlayer* localPlayer) override;
	void onDisable() override;
};