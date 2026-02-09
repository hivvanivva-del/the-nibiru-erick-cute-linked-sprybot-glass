#include "ElytraFly.h"
#include "../../../ModuleManager.h"

ElytraFly::ElytraFly() : Module("ElytraFly", "Flies with elytras", Category::MOVEMENT) {
	registerSetting(new EnumSetting("Mode", "NULL", { "Control", "Jetpack", "Lag" }, &mode, mode));
	registerSetting(new SliderSetting<float>("Speed", "Horizontal speed", &hSpeed, hSpeed, 0.1f, 2.f, [&]() -> bool {
		return mode != 2;
		}));
	registerSetting(new SliderSetting<float>("V-Speed", "Vertical speed", &vSpeed, vSpeed, 0.1f, 2.f, [&]() -> bool {
		return mode != 1;
		}));
}

void ElytraFly::onNormalTick(LocalPlayer* localPlayer) {
	static InventoryMove* invMove = ModuleManager::getModule<InventoryMove>();

	if (localPlayer->isInLiquid()) {
		flying = false;
		return;
	}

	ItemStack* chestPiece = localPlayer->getArmor(1);

	if (!chestPiece || !chestPiece->isValid()) {
		flying = false;
		return;
	}

	if (chestPiece->item.get()->itemId != 580) {
		flying = false;
		return;
	}

	if (!localPlayer->getStatusFlag(ActorFlags::Gliding)) {
		flying = false;
		return;
	}

	flying = true;

	Vec3<float>& velocity = localPlayer->stateVector->velocity;
	if (mode != 1) {
		if (!pressing) {
			velocity.y = 0.f;
		}

		if (Game::canUseMoveKeys() || invMove->isEnabled()) {
			if (!MoveUtil::isMoving()) {
				MoveUtil::stopMotion();
			}
			else {
				MoveUtil::setSpeed(mode == 0 ? hSpeed : Math::randomFloat(0.10f, hSpeed));
			}
		}
	}
	else {
		float calcYaw = (localPlayer->rotation->presentRot.y + 90.f) * (PI / 180.f);
		float calcPitch = (localPlayer->rotation->prevRot.x) * -(PI / 180.f);

		Vec3<float> moveVec;

		if (Game::canUseMoveKeys() || invMove->isEnabled()) {
			moveVec.x = cos(calcYaw) * cos(calcPitch) * hSpeed;
			moveVec.z = sin(calcYaw) * cos(calcPitch) * hSpeed;
		}

		moveVec.y = sin(calcPitch) * hSpeed;

		localPlayer->lerpMotion(moveVec);
	}
}

void ElytraFly::onKey(int key, bool isDown) {
	LocalPlayer* localPlayer = Game::getLocalPlayer();

	if (!localPlayer) {
		return;
	}

	if (mode == 1) {
		return;
	}

	pressing = true;

	if (key == VK_SPACE) {
		localPlayer->lerpMotion(Vec3<float>(0.f, vSpeed, 0.f));
	}
	else if (key == VK_SHIFT) {
		localPlayer->lerpMotion(Vec3<float>(0.f, -vSpeed, 0.f));
	}
}

void ElytraFly::onDisable() {
	flying = false;
}