#pragma once
#include "../../ModuleBase/Module.h"

class DeathLogger : public Module {
public:
	DeathLogger() : Module("DeathLogger", "Logs your death", Category::PLAYER) {

	}

	Vec3<float> lastPos = {};
	void onNormalTick(LocalPlayer* localPlayer) override {
		if (localPlayer->getDeathTime() == 1) {
			Game::DisplayClientMessage("DeathLogger: %sPosition saved at %s%i%s, %s%i%s, %s%i%s.", MCTF::GRAY, MCTF::WHITE, lastPos.x, MCTF::GRAY, MCTF::WHITE, lastPos.y, MCTF::GRAY, MCTF::WHITE, lastPos.z, MCTF::GRAY);
		}
		else {
			lastPos = localPlayer->getPos().floor().sub(0, 1, 0);
		}
	}
};