#pragma once
#include "../../ModuleBase/Module.h"

class FreeLook : public Module {
private:
	Vec2<float> lookAngle;
public:
	FreeLook();

	void onEnable() override;
	void onUpdateRotation(LocalPlayer* localPlayer) override;
};