#pragma once
#include "../../ModuleBase/Module.h"

class AutoCity : public Module {
public:
	std::vector<Actor*> actorList;
private:
	float range = 6.f;
	bool face = false;
	bool burrow = false;
	bool priority = false;
private:
	BlockPos getMinePos(Actor* actor) const;
public:
	AutoCity();
	virtual void onNormalTick(LocalPlayer* localPlayer) override;
};