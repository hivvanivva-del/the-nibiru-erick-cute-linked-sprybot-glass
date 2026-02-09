#pragma once
#include "../../ModuleBase/Module.h"

class AntiPiston final : public Module {
public:
	bool mineRedstone = false;
	bool minePiston = true;
	bool smart = false;
public:
	AntiPiston();
	virtual void onNormalTick(LocalPlayer* localPlayer) override;
};