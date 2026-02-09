#include "LiquidMove.h"

LiquidMove::LiquidMove() : Module("LiquidMove", "Move faster on water", Category::MOVEMENT) {
	registerSetting(new SliderSetting<float>("Speed", "NULL", &speed, 1.f, 0.1f, 2.f));
	registerSetting(new SliderSetting<float>("YSpeed", "NULL", &upSpeed, 1.f, 0.1f, 2.f));
}

void LiquidMove::onNormalTick(LocalPlayer* localPlayer) {
	if (!Game::canUseMoveKeys()) return;
	if (!localPlayer->isInLiquid()) return;
	const bool up = Game::isKeyDown(VK_SPACE);
	const bool down = Game::isKeyDown(VK_SHIFT);
	MoveUtil::setSpeed(speed);
	if (localPlayer->isInWater()) {
		if (up) localPlayer->stateVector->velocity.y += upSpeed / 11.f;
		else if (down) localPlayer->stateVector->velocity.y -= upSpeed / 11.f;
	}
	else if (localPlayer->isInLava()) {
		if (up) localPlayer->stateVector->velocity.y += upSpeed / 4.f;
		else if (down) localPlayer->stateVector->velocity.y -= upSpeed / 4.f;
	}
}