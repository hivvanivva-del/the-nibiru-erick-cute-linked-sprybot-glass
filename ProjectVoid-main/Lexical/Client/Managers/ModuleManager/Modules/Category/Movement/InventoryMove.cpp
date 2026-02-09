#include "InventoryMove.h"
#include "../../../ModuleManager.h"

void InventoryMove::onNormalTick(LocalPlayer* localPlayer) {
	static Fly* flyMod = ModuleManager::getModule<Fly>();
	static Speed* speed = ModuleManager::getModule<Speed>();
	static NoClip* noClip = ModuleManager::getModule<NoClip>();
	static ElytraFly* eFly = ModuleManager::getModule<ElytraFly>();
	
	if (flyMod->isEnabled() || noClip->isEnabled() || (eFly->isEnabled() && eFly->flying) || speed->isEnabled()) {
		return;
	}

	if (Game::canUseMoveKeys()) {
		return;
	}

	MoveUtil::setSpeed(0.2805f);
}