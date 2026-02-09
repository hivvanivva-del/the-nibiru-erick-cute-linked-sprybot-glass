#include "Disabler.h"

Disabler::Disabler() : Module("Disabler", "Disable some anticheats", Category::WORLD) {
	registerSetting(new EnumSetting("Mode", "Type of disabler mode", { "Geyser" }, &mode, 0));
}

std::string Disabler::getModeText() {
	return mode == 0 ? "Geyser" : "NULL";
}

void Disabler::onNormalTick(LocalPlayer* localPlayer) {
}

void Disabler::onBuild(const BlockPos& blockPos, const uint8_t face, bool& cancel) {
	if (mode == 0) Game::getLocalPlayer()->level->getHitResult()->type = HitResultType::AIR;
}

void Disabler::onAttack(Actor* actor, bool& cancel) {
	if (mode == 0) Game::getLocalPlayer()->level->getHitResult()->type = HitResultType::AIR;
}