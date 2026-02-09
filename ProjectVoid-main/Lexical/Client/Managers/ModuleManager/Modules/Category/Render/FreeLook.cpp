#include "FreeLook.h"

FreeLook::FreeLook() : Module("FreeLook", "Move camera in 3rd person but not change player rotation", Category::RENDER) {
}

void FreeLook::onEnable() {
	LocalPlayer* localPlayer = Game::getLocalPlayer();
	if (localPlayer != nullptr)
		lookAngle = localPlayer->rotation->presentRot;
}

void FreeLook::onUpdateRotation(LocalPlayer* localPlayer) {
	ActorRotationComponent* rotation = localPlayer->rotation;
	ActorHeadRotationComponent* headRot = localPlayer->getActorHeadRotationComponent();

	if (rotation == nullptr)
		return;

	if (headRot == nullptr)
		return;

	rotation->presentRot = lookAngle;
	headRot->headYaw = lookAngle.y;
}