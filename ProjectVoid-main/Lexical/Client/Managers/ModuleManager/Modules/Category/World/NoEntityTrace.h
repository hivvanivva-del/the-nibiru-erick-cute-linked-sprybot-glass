#pragma once
#include "../../ModuleBase/Module.h"

class NoEntityTrace : public Module {
public:
	NoEntityTrace() : Module("NoEntityTrace", "Mines through entities", Category::WORLD) {

	}
public:
	virtual void onNormalTick(LocalPlayer* lp) override {
		for (Actor* actor : lp->level->getRuntimeActorList()) {
			if (!TargetUtil::isTargetValid(actor, false, true)) continue;
			actor->aabbShape->width = 0.f;
		}
	}
	virtual void onDisable() override {
		if (!Game::getLocalPlayer()) return;
		for (Actor* actor : Game::getLocalPlayer()->level->getRuntimeActorList()) {
			if (!TargetUtil::isTargetValid(actor, false, true)) continue;
			actor->aabbShape->width = 0.6f;
		}
	}
};