#pragma once
#include "../../ModuleBase/Module.h"

class ElytraFly : public Module {
private:
	int mode = 0;
	float hSpeed = 1.f;
	float vSpeed = 1.f;
public:
	bool flying = false;
	bool pressing = false;
public:
	ElytraFly();

	void onNormalTick(LocalPlayer* localPlayer) override;
	void onDisable() override;

	void onKey(int key, bool isDown);
};