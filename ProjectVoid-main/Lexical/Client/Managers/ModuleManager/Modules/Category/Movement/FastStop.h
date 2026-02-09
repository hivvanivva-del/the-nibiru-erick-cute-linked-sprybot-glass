#pragma once
#include "../../ModuleBase/Module.h"
#include "../../../ModuleManager.h"

class FastStop : public Module {
public:
	FastStop() : Module("FastStop", "Stops fast", Category::MOVEMENT) {

	}
	virtual void onNormalTick(LocalPlayer* localPlayer) override;
};