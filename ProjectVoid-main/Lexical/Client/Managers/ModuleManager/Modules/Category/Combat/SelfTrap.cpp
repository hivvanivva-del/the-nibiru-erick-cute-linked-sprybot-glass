#include "SelfTrap.h"

SelfTrap::SelfTrap() : Module("SelfTrap", "Trap self NOW!", Category::COMBAT) {
	registerSetting(new EnumSetting("Mode", "NULL", { "Normal", "Minimal" }, &mode, 0));
	registerSetting(new SliderSetting<int>("BPT", "Blocks place per tick", &bpt, 1, 1, 10));
	registerSetting(new BoolSetting("AirPlace", "Place on air", &airplace, false));
	registerSetting(new SliderSetting<int>("Head Space", "Amount of head allowance", &headSpace, 0, 0, 3));
	registerSetting(new EnumSetting("Switch Mode", "Switch Type\nNone: Dont switch\nNormal: Switch to end crystal\nSilent: Switches silently\nSpoof: Uses MobEquipmentPackets to switch to end crystal, 'real silent'", { "None", "Normal", "Silent", "Spoof" }, &switchMode, 0));
	registerSetting(new BoolSetting("Packet Place", "Place using packets", &packetPlace, false));
	registerSetting(new BoolSetting("Button", "Places button above and below u", &button, false));
}

void SelfTrap::onNormalTick(LocalPlayer* localPlayer) {
	static std::vector<Actor*> entityList;
	static std::vector<BlockPos> placeList;
	static std::vector<BlockPos> buttonList;
	BlockSource* region = Game::clientInstance->getRegion();
	if (region == nullptr) return;
	entityList.clear();
	buttonList.clear();
	if (localPlayer->getItemUseDuration() > 0) return;
	entityList.push_back(localPlayer);
	for (Actor* actor : localPlayer->level->getRuntimeActorList()) {
		if (!TargetUtil::isTargetValid(actor, true)) continue;
		if (actor->getActorTypeComponent()->id == 71) continue;
		entityList.push_back(actor);
	}
	uint8_t obsidianSlot = InventoryUtil::findItemSlotInHotbar(49);
	uint8_t buttonSlot = InventoryUtil::findItemSlotInHotbar(143);
	if (!placeList.empty()) placeList.clear();
	int xStart = -1;
	int zStart = -1;
	int xEnd = 1;
	int zEnd = 1;
	AABB localAABB = localPlayer->aabbShape->aabb;
	Vec3<float> feetPos = localAABB.getCenter(); feetPos.y = localAABB.lower.y;
	BlockPos actorPos = feetPos.add(Vec3<float>(0.f, 0.5f, 0.f)).floor().CastTo<int>();
	if (PlayerUtil::canPlaceBlock(localPlayer->getPos().sub(0, 1.6f, 0).CastTo<int>(), airplace)) buttonList.push_back(localPlayer->getPos().sub(0, 1.6f, 0).CastTo<int>());
	if (mode == 0) {
		for (Actor* actor : entityList) {
			AABB actorAABB = actor->aabbShape->aabb;
			if (actor->getActorTypeComponent()->id == 319) actorAABB = TargetUtil::makeAABB(0.6f, actor->aabbShape->height, actor->getEyePos());
			if (actorAABB.intersects(getBlockAABB(actorPos.add(BlockPos(xStart, 0, 0))))) xStart -= 1;
			if (actorAABB.intersects(getBlockAABB(actorPos.add(BlockPos(0, 0, zStart))))) zStart -= 1;
			if (actorAABB.intersects(getBlockAABB(actorPos.add(BlockPos(xEnd, 0, 0))))) xEnd += 1;
			if (actorAABB.intersects(getBlockAABB(actorPos.add(BlockPos(0, 0, zEnd))))) zEnd += 1;
		}

		for (int x = xStart; x <= xEnd; x++) {
			for (int z = zStart; z <= zEnd; z++) {
				if (x == xStart && z == zStart) continue;
				if (x == xStart && z == zEnd) continue;
				if (x == xEnd && z == zEnd) continue;
				if (x == xEnd && z == zStart) continue;

				if (x > xStart && x < xEnd && z > zStart && z < zEnd) {
					const BlockPos& placePos = actorPos.add(BlockPos(x, -1, z));
					const BlockPos& abovePos = actorPos.add(BlockPos(x, 2 + headSpace, z));
					if (PlayerUtil::canPlaceBlock(placePos, airplace)) placeList.push_back(placePos);
					if (PlayerUtil::canPlaceBlock(abovePos, airplace)) placeList.push_back(abovePos);
					if (PlayerUtil::canPlaceBlock(abovePos.sub(0, 1, 0), airplace)) buttonList.push_back(abovePos.sub(0, 1, 0));
					continue;
				}
				for (int i = 0; i < 3 + headSpace; i++) {
					const BlockPos& placePos = actorPos.add(BlockPos(x, i, z));
					if (!WorldUtil::getBlock(placePos)->blockLegacy->canBeBuiltOver(region, placePos)) continue;
					if (PlayerUtil::canPlaceBlock(placePos, airplace)) placeList.push_back(placePos);
				}
				for (int i = 0; i < 3 + headSpace; i++) {
					const BlockPos& placePos = BlockPos(actorPos.x + x, actorPos.y + i - 1, actorPos.z + z);
					if (!WorldUtil::getBlock(placePos)->blockLegacy->canBeBuiltOver(region, placePos)) continue;
					if (PlayerUtil::canPlaceBlock(placePos, airplace)) placeList.push_back(placePos);
				}
			}
		}
	}
	else {
		const BlockPos& abovePos = BlockPos(actorPos.x, actorPos.y + 2, actorPos.z);
		for (int i = 0; i < 3; i++) {
			const BlockPos& supportPos = BlockPos(actorPos.x + 1, actorPos.y + i, actorPos.z);
			if (PlayerUtil::canPlaceBlock(supportPos, airplace)) placeList.push_back(supportPos);
		}
		if (PlayerUtil::canPlaceBlock(abovePos, airplace)) placeList.push_back(abovePos);
	}
	if (!placeList.empty() || !buttonList.empty()) {
		uint8_t oldSlot = InventoryUtil::getSelectedSlot();
		if (!placeList.empty()) {
			std::sort(placeList.begin(), placeList.end(), [localPlayer](const BlockPos& a1, const BlockPos& a2) {
				AABB playerAABB = localPlayer->aabbShape->aabb;
				Vec3<float> feetPos = playerAABB.getCenter();
				feetPos.y = playerAABB.lower.y;
				return (WorldUtil::distanceToBlock(feetPos, a1) < WorldUtil::distanceToBlock(feetPos, a2));
				}
			);

			if (switchMode != 0) {
				if (switchMode == 1 || switchMode == 2) InventoryUtil::switchSlot(obsidianSlot);
				if (switchMode == 3 && PlayerUtil::selectedSlotServerSide != obsidianSlot) InventoryUtil::sendMobEquipment(obsidianSlot);
			}

			if (InventoryUtil::getSelectedSlot() == obsidianSlot || switchMode == 3) {
				int placed = 0;
				for (BlockPos& blockPos : placeList) {
					if (PlayerUtil::tryPlaceBlock(blockPos, airplace, packetPlace)) placed++;
					if (placed >= bpt) break;
				}
			}
		}
		if (!buttonList.empty()) {
			if (switchMode != 0) {
				if (switchMode == 1 || switchMode == 2) InventoryUtil::switchSlot(buttonSlot);
				if (switchMode == 3 && PlayerUtil::selectedSlotServerSide != buttonSlot) InventoryUtil::sendMobEquipment(buttonSlot);
			}
			if (InventoryUtil::getSelectedSlot() == buttonSlot || switchMode == 3) {
				for (BlockPos& blockPos : buttonList) {
					PlayerUtil::tryPlaceBlock(blockPos, airplace, packetPlace);
				}
			}
		}

		if (switchMode == 3 || switchMode == 2) {
			if (switchMode == 2) InventoryUtil::switchSlot(oldSlot);
			else InventoryUtil::sendMobEquipment(oldSlot);
		}
	}
}