#include "AutoHoleFill.h"
#include "../../../ModuleManager.h"

AutoHoleFill::AutoHoleFill() : Module("HoleFill", "Fills holes automatically", Category::COMBAT) {
	registerSetting(new SliderSetting<int>("Place Range", "NULL", &placeRange, 1, 1, 12));
	registerSetting(new SliderSetting<int>("Wall Range", "NULL", &wallRange, 1, 1, 12));
	registerSetting(new SliderSetting<int>("Proximity", "NULL", &proximity, 6, 1, 12));
	registerSetting(new BoolSetting("Packet Place", "Place using packets preventing ghost blocks", &packetPlace, true));
	registerSetting(new BoolSetting("EatStop", "Stop when eating", &eatStop, true));
	registerSetting(new SliderSetting<int>("Delay", "Delay of holefiller", &placeDelay, 1, 0, 20));
	registerSetting(new SliderSetting<int>("BPT", "Amount of blocks to place per tick", &blocksPerTick, 4, 1, 10));
	registerSetting(new BoolSetting("Smart", "Fills holes smartly", &smart, true));
	registerSetting(new SliderSetting<float>("Target Range", "NULL", &targetRange, 12.f, 1.f, 20.f, [&]() -> bool {
		return smart;
		}
	));
	registerSetting(new SliderSetting<float>("Target Distance", "Will only fill holes if the distance to them is less than this", &targetDist, 4.f, 1.f, 12.f, [&]() -> bool {
		return smart;
		}
	));
	registerSetting(new SliderSetting<float>("Target WallDist", "Will use this instead of target distance if theres a wall between hole and target", &targetDistWall, 4.f, 1.f, 12.f, [&]() -> bool {
		return smart;
		}
	));
	registerSetting(new BoolSetting("Raytrace", "Wont fill the holes that cant be seen by the target", &raytrace, false, [&]() -> bool {
		return smart;
		})
	);
	registerSetting(new BoolSetting("Absolute Raytrace", "If this is off then it wont fill the holes behind them, only in front of them and in the side\nif this is on then it will only fill the holes in front of them", &absoluteRaytrace, false, [&]() -> bool {
		return smart && raytrace;
		})
	);
	registerSetting(new SliderSetting<int>("Max Holes", "Maximum amount of holes, set to 0 for no limit", &maxHoles, 0, 0, 20, [&]() -> bool {
		return smart;
		}
	));
	registerSetting(new BoolSetting("Predict", "Predict targets next position", &predict, false, [&]() -> bool {
		return smart;
		})
	);
	registerSetting(new SliderSetting<float>("Predict Val", "Value of prediction", &predictIntensity, predictIntensity, 1.f, 20.f, [&]() -> bool {
		return smart && predict;
		})
	);
	registerSetting(new SliderSetting<float>("Safe Distance", "Wont fill holes that are this near to you", &safeDistance, 2.f, 0.f, 12.f, [&]() -> bool {
		return smart;
		}
	));
	registerSetting(new BoolSetting("Safe", "Only fills holes when you are in hole", &safe, false, [&]() -> bool {
		return smart;
		})
	);
	registerSetting(new EnumSetting("Block Type", "NULL", { "Obsidian", "E-Chest", "Any" }, &blockMode, 0));
	registerSetting(new BoolSetting("MobEq", "Send mobEq packet", &mobEq, false));
	registerSetting(new BoolSetting("Render", "NULL", &render, false));
	registerSetting(new ColorSetting("Color", "NULL", &renderColor, renderColor));
}

bool AutoHoleFill::isValidHole(const BlockPos& blockPos) {
	static HoleESP* holeEsp = ModuleManager::getModule<HoleESP>();
	return holeEsp->getHoleType(blockPos) != HoleESP::HoleType::Invalid;
}

bool AutoHoleFill::canFillHole(const BlockPos& blockPos) {
	LocalPlayer* lp = Game::getLocalPlayer();
	AABB blockAABB = AABB(blockPos.CastTo<float>(), blockPos.CastTo<float>().add(1.f));
	float distance = WorldUtil::distanceToBlock(lp->getEyePos(), blockPos);
	if (distance > placeRange) return false;
	for (Actor* actor : entityList) {
		if (!TargetUtil::isTargetValid(actor, true, false) && actor != lp) continue;
		AABB actorAABB = actor->aabbShape->aabb;
		if (actor->getActorTypeComponent()->id == 319) {
			actorAABB = TargetUtil::makeAABB(0.6f, actor->aabbShape->height, actor->getEyePos());
		}
		if (actorAABB.intersects(blockAABB)) return false;
	}
	if (!PlayerUtil::canPlaceBlock(blockPos)) return false;
	return true;
}

void AutoHoleFill::getHoleList(LocalPlayer* localPlayer) {
	Vec3<float> actorPos = localPlayer->getEyePos().sub(0.f, 1.6f, 0.f);
	for (int x = -proximity; x <= proximity; x++) {
		for (int y = -proximity; y <= proximity; y++) {
			for (int z = -proximity; z <= proximity; z++) {
				const BlockPos& blockPos = actorPos.CastTo<int>().add(x, y, z);
				if (isValidHole(blockPos)) {
					if (WorldUtil::getBlock(blockPos)->blockLegacy->blockId != 143) holeList.push_back(blockPos);
					else holeList.push_back(blockPos.add(0, 1, 0));
				}
			}
		}
	}
}

bool AutoHoleFill::isInRenderList(const BlockPos& blockPos) {
	if (renderList.empty()) return false;
	for (const std::pair<BlockPos, float> pair : renderList) {
		if (pair.first == blockPos) return true;
	}
	return false;
}

void AutoHoleFill::fillHole(const std::vector<BlockPos>& vector) {
	uint8_t selectedSlot = InventoryUtil::getSelectedSlot();
	if (blockMode == 0 && InventoryUtil::getHeldItemId() != 49) return;
	else if (blockMode == 1 && InventoryUtil::getHeldItemId() != 130) return;
	else if (blockMode == 2 && !InventoryUtil::isBlock(InventoryUtil::getItem(selectedSlot))) return;
	int c = 0;
	static float d = 0;
	if (TimerUtil::hasReached<ticks>(&d, placeDelay)) {
		for (const BlockPos& blockPos : vector) {
			if (!canFillHole(blockPos)) continue;
			if (!isInRenderList(blockPos)) renderList.push_back(std::make_pair(blockPos, 1.f));
			if (PlayerUtil::tryPlaceBlock(blockPos, false, packetPlace)) c++;
			if (c >= blocksPerTick) break;
		}
	}
}

uint8_t AutoHoleFill::getBlock() {
	int32_t result = -1;
	for (int i = 0; i < 9; i++) {
		ItemStack* itemStack = Game::getLocalPlayer()->playerInventory->container->getItem(i);
		if (itemStack->isValid() && itemStack->isBlock()) {
			result = i;
			break;
		}
	}
	return result;
}

AutoHoleFill::Face AutoHoleFill::getFacing(Actor* actor) {
	float rotation = actor->rotation->presentRot.y;
	if (rotation < 0) rotation += 360.0;
	if ((0 <= rotation && rotation < 45) || (315 <= rotation && rotation < 360)) return AutoHoleFill::Face::ZPositive;
	else if (45 <= rotation && rotation < 135) return AutoHoleFill::Face::XNegative;
	else if (135 <= rotation && rotation < 225) return AutoHoleFill::Face::ZNegative;
	else if (225 <= rotation && rotation < 315) return AutoHoleFill::Face::ZPositive;
	return AutoHoleFill::Face::Unknown;
}

AutoHoleFill::Face AutoHoleFill::getDirection(const Vec3<float>& a, const BlockPos& b) {
	Vec3<float> direction = Vec3<float>(b.x - a.x, 0, b.z - a.z);
	float magnitude = direction.magnitude();
	if (magnitude != 0) {
		direction.x /= magnitude;
		direction.z /= magnitude;
	}
	if (std::abs(direction.z) > std::abs(direction.x)) {
		if (direction.z > 0) return AutoHoleFill::Face::ZPositive;
		else return AutoHoleFill::Face::ZNegative;
	}
	else {
		if (direction.z > 0) return AutoHoleFill::Face::XPositive;
		else return AutoHoleFill::Face::XNegative;
	}
	return AutoHoleFill::Face::Unknown;
}

void AutoHoleFill::onNormalTick(LocalPlayer* localPlayer) {
	entityList.clear();
	targetList.clear();
	holeList.clear();
	holes.clear();
	if (eatStop && localPlayer->getItemUseDuration() > 0) return;
	for (Actor* actor : localPlayer->level->getRuntimeActorList()) {
		if (!actor) continue;
		entityList.push_back(actor);
		if (!TargetUtil::isTargetValid(actor, false, true, targetRange)) continue;
		targetList.push_back(actor);
	}
	getHoleList(localPlayer);
	if (holeList.empty()) return;
	uint8_t oldSlot = InventoryUtil::getSelectedSlot();
	if ((!smart) || (smart && !targetList.empty())) {
		if (blockMode == 0) {
			InventoryUtil::switchSlot(InventoryUtil::findItemSlotInHotbar(49));
			if (mobEq) InventoryUtil::sendMobEquipment(InventoryUtil::findItemSlotInHotbar(49));
		}
		else if (blockMode == 1) {
			InventoryUtil::switchSlot(InventoryUtil::findItemSlotInHotbar(130));
			if (mobEq) InventoryUtil::sendMobEquipment(InventoryUtil::findItemSlotInHotbar(130));
		}
		else if (blockMode == 2) {
			InventoryUtil::switchSlot(getBlock());
			if (mobEq) InventoryUtil::sendMobEquipment(getBlock());
		}
	}
	Vec3<float> localPos = localPlayer->getPos().floor().sub(0, 1, 0);
	std::sort(holeList.begin(), holeList.end(), [localPos](const BlockPos& b1, const BlockPos& b2) {
		return WorldUtil::distanceToBlock(localPos, b1) < WorldUtil::distanceToBlock(localPos, b2);
	});
	if (!smart) fillHole(holeList);
	else {
		if (targetList.empty()) return;
		std::sort(targetList.begin(), targetList.end(), TargetUtil::sortByDist);
		Actor* mainActor = nullptr;
		for (Actor* actor : targetList) { // ok putting all teh code below in for loop causes kick
			if (isValidHole(actor->getPos().floor().sub(0, 1, 0).CastTo<int>())) continue;
			mainActor = actor;
			break;
		}
		if (mainActor) {
			Vec3<float> actorPos = mainActor->getPos().floor().sub(0, 1, 0);
			Vec3<float> velo = mainActor->stateVector->velocity;
			Vec3<float> predictedPos = actorPos.add(predict ? velo.mul(predictIntensity) : 0.f);
			for (const BlockPos& block : holeList) {
				float distance = WorldUtil::distanceToBlock(predictedPos, block);
				float localDistance = WorldUtil::distanceToBlock(localPos, block);
				float exposure = WorldUtil::getSeenPercent(block.CastTo<float>(), mainActor);
				if (block.y > actorPos.y) continue;
				if (!canFillHole(block)) continue;
				if (raytrace) {
					AutoHoleFill::Face face = getFacing(mainActor);
					AutoHoleFill::Face direction = getDirection(actorPos, block);
					if (!absoluteRaytrace) {
						if (face == AutoHoleFill::Face::XPositive && direction == AutoHoleFill::Face::XNegative) continue;
						else if (face == AutoHoleFill::Face::XNegative && direction == AutoHoleFill::Face::XPositive) continue;
						else if (face == AutoHoleFill::Face::ZPositive && direction == AutoHoleFill::Face::ZNegative) continue;
						else if (face == AutoHoleFill::Face::ZNegative && direction == AutoHoleFill::Face::ZPositive) continue;
					}
					else {
						if (face != direction) continue;
					}
					if (exposure == 0.f) continue;
				}
				if (exposure == 0.f) {
					if (distance > targetDistWall) continue;
				}
				else {
					if (distance > targetDist) continue;
				}
				if (isValidHole(localPos.CastTo<int>()) || (!isValidHole(localPos.CastTo<int>()) && localDistance > safeDistance)) holes.push_back(block);
			}
			if (!holes.empty()) {
				std::sort(holes.begin(), holes.end(), [&](const BlockPos& start, const BlockPos& end) {
					return WorldUtil::distanceToBlock(predictedPos, start) < WorldUtil::distanceToBlock(predictedPos, end); }
				);
				if (maxHoles != 0 && holes.size() > maxHoles) {
					for (int i = holes.size(); i > 0; i--) {
						holes.erase(holes.begin() + i);
					}
				}
				if (!safe || (safe && isValidHole(localPos.CastTo<int>()))) fillHole(holes);
			}
		}
	}
	InventoryUtil::switchSlot(oldSlot);
	if (mobEq && PlayerUtil::selectedSlotServerSide != oldSlot) InventoryUtil::sendMobEquipment(oldSlot);
}

void AutoHoleFill::onLevelRender() {
	if (!render) return;
	LocalPlayer* localPlayer = Game::getLocalPlayer();
	for (auto it = renderList.begin(); it != renderList.end(); ) {
		if (it->second > 0.f) {
			MCR::drawBox3dFilled(AABB(it->first.CastTo<float>(), it->first.CastTo<float>().add(1.f, 0.1f, 1.f)), UIColor(renderColor.r, renderColor.g, renderColor.b, (int)(renderColor.a * it->second)), UIColor(renderColor.r, renderColor.g, renderColor.b, (int)(renderColor.a * it->second)));
			if (!canFillHole(it->first)) it->second -= MCR::deltaTime * 1.5f;
			it++;
		}
		else it = renderList.erase(it);
	}
}