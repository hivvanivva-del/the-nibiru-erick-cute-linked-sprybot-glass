#include "MovementFix.h"
#include "../../../ModuleManager.h"

MovementFix::MovementFix() : Module("MovementFix", "Smoother movement!", Category::MOVEMENT) {

}

void MovementFix::onNormalTick(LocalPlayer* localPlayer) {
	static Speed* speedMod = ModuleManager::getModule<Speed>();
	static Fly* flyMod = ModuleManager::getModule<Fly>();
	static NoClip* noClip = ModuleManager::getModule<NoClip>();
	static Jetpack* jetPack = ModuleManager::getModule<Jetpack>();
	if (!Game::canUseMoveKeys()) return;
	if (localPlayer->isInLiquid()) return;
	if (speedMod->isEnabled() or flyMod->isEnabled() or noClip->isEnabled() or jetPack->isEnabled()) return;
	float speed = 0.15f;
	if (localPlayer->onGround()) speed = 0.15f;
	else speed = Math::randomFloat(0.28f, 0.30f);
	MoveUtil::setSpeed(speed);
}