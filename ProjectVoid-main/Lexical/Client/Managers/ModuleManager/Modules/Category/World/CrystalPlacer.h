#pragma once
#include "../../ModuleBase/Module.h"

class CrystalPlacer : public Module {
public:
	std::vector<Actor*> entityList;
public:
	CrystalPlacer();
	virtual void onNormalTick(LocalPlayer* localPlayer) override;
};