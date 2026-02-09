#include "AutoTrap.h"

AutoTrap::AutoTrap() : Module("AutoTrap", "Traps people", Category::COMBAT) {
	registerSetting(new SliderSetting<int>("Range", "NULL", &range, 6, 1, 12));
	registerSetting(new SliderSetting<int>("BPT", "Blocks place per tick", &bpt, 1, 1, 10));
	registerSetting(new SliderSetting<int>("Targets", "Amount of people to trap in 1 go", &maxTarget, 1, 1, 10));
	registerSetting(new BoolSetting("AirPlace", "Place on air", &airplace, false));
	registerSetting(new SliderSetting<int>("Head Space", "Amount of head allowance", &headSpace, 0, 0, 3));
	registerSetting(new EnumSetting("Switch Mode", "Switch Type\nNone: Dont switch\nNormal: Switch to end crystal\nSilent: Switches silently\nSpoof: Uses MobEquipmentPackets to switch to end crystal, 'real silent'", { "None", "Normal", "Silent", "Spoof" }, &switchMode, 0));
	registerSetting(new BoolSetting("Packet Place", "Place using packets", &packetPlace, false));
	registerSetting(new ColorSetting("Color", "NULL", &color, UIColor(255, 255, 255, 40)));
}

void AutoTrap::onNormalTick(LocalPlayer* player) {
	static std::vector<Actor*> entityList;
	static std::vector<Actor*> targetList;
	BlockSource* region = Game::clientInstance->getRegion();
	if (region == nullptr) return;
	entityList.clear();
	targetList.clear();
	entityList.push_back(player);
	for (Actor* actor : player->level->getRuntimeActorList()) {
		if (!TargetUtil::isTargetValid(actor, true)) continue;
		if (actor->getActorTypeComponent()->id == 71) continue;
		entityList.push_back(actor);
		if (!TargetUtil::isTargetValid(actor, false, true, (float)range)) continue;
		targetList.push_back(actor);
	}
	int32_t& selectedSlot = player->playerInventory->selectedSlot;
	int obsidianSlot = InventoryUtil::findItemSlotInHotbar(49);
	if (!placeList.empty()) placeList.clear();
	else {
		if (oldSlot != -1) {
			selectedSlot = oldSlot;
			oldSlot = -1;
		}
	}

	int maxTarg = 0;
	if (targetList.empty()) return;
	for (Actor* actor : targetList) {
		int xStart = -1;
		int zStart = -1;
		int xEnd = 1;
		int zEnd = 1;
		AABB targetAABB = actor->aabbShape->aabb;
		Vec3<float> feetPos = targetAABB.getCenter(); feetPos.y = targetAABB.lower.y;
		BlockPos actorPos = feetPos.add(Vec3<float>(0.f, 0.5f, 0.f)).floor().CastTo<int>();
		for (Actor* entity : entityList) {
			AABB actorAABB = entity->aabbShape->aabb;
			if (entity->getActorTypeComponent()->id == 319) actorAABB = TargetUtil::makeAABB(0.6f, entity->aabbShape->height, entity->getEyePos());
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
		if (!placeList.empty()) {
			std::sort(placeList.begin(), placeList.end(), [actor](const BlockPos& a1, const BlockPos& a2) {
				AABB playerAABB = actor->aabbShape->aabb;
				Vec3<float> feetPos = playerAABB.getCenter();
				feetPos.y = playerAABB.lower.y;
				return (WorldUtil::distanceToBlock(feetPos, a1) < WorldUtil::distanceToBlock(feetPos, a2));
				}
			);
		}
		if (maxTarg++ >= maxTarget) break;
	}

	if (!placeList.empty()) {
		if (switchMode != 0) {
			if ((switchMode == 1 || switchMode == 2) && oldSlot == -1) {
				oldSlot = selectedSlot;
				selectedSlot = obsidianSlot;
			}
			if (switchMode == 3 && PlayerUtil::selectedSlotServerSide != obsidianSlot) InventoryUtil::sendMobEquipment(obsidianSlot);
		}

		if (selectedSlot == obsidianSlot || switchMode == 3) {
			int placed = 0;
			for (BlockPos& blockPos : placeList) {
				if (PlayerUtil::tryPlaceBlock(blockPos, airplace, packetPlace)) {
					fadeList[blockPos] = 1.f;
					placed++;
				}
				if (placed >= bpt) break;
			}
		}

		if (switchMode == 3 || switchMode == 2) {
			if (switchMode == 2) selectedSlot = oldSlot;
			else {
				InventoryUtil::sendMobEquipment(oldSlot);
			}
			oldSlot = -1;
		}
	}
}

void AutoTrap::onLevelRender() {
	for (BlockPos& blockPos : placeList) {
		MCR::drawBox3dFilled(getBlockAABB(blockPos), color, color);
	}

	for (auto it = fadeList.begin(); it != fadeList.end(); ) {
		if (it->second > 0.f) {
			MCR::drawBox3dFilled(getBlockAABB(it->first), UIColor(color.r, color.g, color.b, (int)(color.a * it->second)), UIColor(color.r, color.g, color.b, (int)(color.a * it->second)));
			it->second -= MCR::deltaTime * 1.5f;
			it++;
		}
		else {
			it = fadeList.erase(it);
		}
	}
}