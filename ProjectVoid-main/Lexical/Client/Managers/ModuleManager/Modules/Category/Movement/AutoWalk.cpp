#include "AutoWalk.h"
#include "../../../Lexical/Client/Managers/HookManager/Hooks/Input/KeyMapHook.h"

AutoWalk::AutoWalk() : Module("AutoWalk", "Moves forward automatically", Category::MOVEMENT) {
	registerSetting(new SliderSetting<float>("Speed", "NULL", &speed, speed, 1.f, 2.f));
}

void AutoWalk::onNormalTick(LocalPlayer* localPlayer) {
	static bool wasOnGround = false;
	float calcYaw = (localPlayer->rotation->presentRot.y + 90.f) * (PI / 180.f);
	float calcPitch = 30.f * -(PI / 180.f);

	if (Game::isKeyDown('W')) {
		return;
	}

	Vec3<float> moveVec;
	moveVec.x = cos(calcYaw) * cos(calcPitch) * (0.30f * speed);
	moveVec.y = sin(calcPitch) * 0.30f;
	moveVec.z = sin(calcYaw) * cos(calcPitch) * (0.30f * speed);

	localPlayer->lerpMotion(moveVec);
	
	if (localPlayer->onGround()) {
		wasOnGround = true;
	}
	if (wasOnGround && !localPlayer->onGround()) {
		wasOnGround = false;
	}
	if (!Game::isKeyDown(VK_SPACE) && wasOnGround) localPlayer->stateVector->velocity.y = -1;
}

void AutoWalk::onDisable() {
}