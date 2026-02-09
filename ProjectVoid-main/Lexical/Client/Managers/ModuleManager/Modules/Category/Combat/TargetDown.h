#pragma once
#include "../../ModuleBase/Module.h"

class TargetDown : public Module {
public:
	std::vector<Actor*> targetList;
private:
	float targetRange = 12.f;
	bool priority = false;
public:
	TargetDown() : Module("TargetDown", "Mines below them", Category::COMBAT) {
		registerSetting(new SliderSetting<float>("Target Range", "Target Range!", &targetRange, 12.f, 1.f, 12.f));
		registerSetting(new BoolSetting("Priority", "NULL", &priority, false));
	}

	virtual void onNormalTick(LocalPlayer* localPlayer) override;
};