#include "FastStop.h"
#include "../../../ModuleManager.h"

void FastStop::onNormalTick(LocalPlayer* localPlayer) {
	static Jetpack* jetPack = ModuleManager::getModule<Jetpack>();
	if (jetPack->isEnabled()) return;
	if (!MoveUtil::isMoving()) MoveUtil::stopMotion();
}