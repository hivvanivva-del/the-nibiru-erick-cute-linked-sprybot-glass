#pragma once
#include "../../ModuleBase/Module.h"

class Disabler : public Module {
public:
	int mode = 0;
public:
	Disabler();

	std::string getModeText() override;
	void onNormalTick(LocalPlayer* localPlayer) override;
	void onBuild(const BlockPos& blockPos, const uint8_t face, bool& cancel) override;
	void onAttack(Actor* actor, bool& cancel) override;
};