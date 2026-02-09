#pragma once
#include "../../ModuleBase/Module.h"

class MovementFix : public Module {
public:
	MovementFix();
public:
	virtual void onNormalTick(LocalPlayer* localPlayer) override;
};