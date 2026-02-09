#pragma once
#include "../../ModuleBase/Module.h"

class SpiderTower : public Module {
private:
	float spiderSpeed = 0.35f;
	bool chestBelow = false;
	int height = 3;
	int bpt = 4;
	bool packetPlace = false;
	bool mobEq = false;
private:
	BlockPos getOffset(LocalPlayer* localPlayer);
public:
	SpiderTower();
	virtual void onNormalTick(LocalPlayer* localPlayer) override;
};