#pragma once
#include "../../ModuleBase/Module.h"

class LiquidMove : public Module {
private:
	float speed = 1.f;
	float upSpeed = 1.f;
public:
	LiquidMove();
	virtual void onNormalTick(LocalPlayer* localPlayer) override;
};