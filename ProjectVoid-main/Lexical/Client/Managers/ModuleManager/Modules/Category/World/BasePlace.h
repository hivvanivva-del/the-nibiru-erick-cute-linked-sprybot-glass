#pragma once
#include "../../ModuleBase/Module.h"

class BasePlace : public Module {
public:
	std::vector<Actor*> targetList;
public:
	bool support = false;
	int proximity = 5;
	int placeRange = 6;
	float targetRange = 6.f;
	int flattenRadius = 2;
	bool self = false;
	bool mobEq = false;
	int placeDelay = 0;
	int iPlaceDelay = 0;
public:
	bool buildBlock(const BlockPos& pos, Actor* actor);
public:
	BasePlace();
	virtual void onNormalTick(LocalPlayer* localPlayer) override;
};