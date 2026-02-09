#include "AnchorAura.h"

AnchorAura::AnchorAura() : Module("AnchorAura", "Places and explodes anchors", Category::COMBAT) {
	registerSetting(new SliderSetting<float>("Target Range", "NULL", &targetRange, 12.f, 1.f, 12.f));
	registerSetting(new SliderSetting<float>("Place Range", "NULL", &placeRange, 6.f, 1.f, 12.f));
	registerSetting(new SliderSetting<int>("Place Delay", "NULL", &placeDelay, 1, 0, 20));
	registerSetting(new SliderSetting<int>("Ignite Delay", "NULL", &igniteDelay, 1, 0, 20));
	registerSetting(new SliderSetting<int>("Detonate Delay", "NULL", &detonateDelay, 1, 0, 20));
	registerSetting(new BoolSetting("Faceplace", "Places on their faces", &faceplace, false));
	registerSetting(new BoolSetting("Hardcode", "Old placements", &hardcode, false));
	registerSetting(new SliderSetting<float>("Min Damage", "NULL", &minDamage, minDamage, 1.f, 20.f, [&]() -> bool {
		return !hardcode;
	}));
	registerSetting(new SliderSetting<int>("Proximity", "NULL", &proximity, proximity, 0, 6, [&]() -> bool {
		return !hardcode;
	}));
	registerSetting(new BoolSetting("Java", "Java logic", &java, false));
	registerSetting(new BoolSetting("EatStop", "EatStop", &eatStop, true));
	registerSetting(new BoolSetting("Silent", "NULL", &silent, true));
	registerSetting(new ColorSetting("Render Color", "NULL", &renderColor, renderColor));
	registerSetting(new BoolSetting("Self Test", "NULL", &self, false));
}

bool AnchorAura::isValid(const BlockPos& blockPos, Actor* actor, LocalPlayer* localPlayer) {
	if (!actor || !localPlayer || !WorldUtil::getBlock(blockPos)) {
		return false;
	}
	if (actor->getPos().dist(localPlayer->getPos()) > placeRange) return false;
	BlockLegacy* block = WorldUtil::getBlock(blockPos)->blockLegacy;
	if (!block) return false;
	if (block->blockId == 527) return true;
	if (!PlayerUtil::canPlaceBlock(blockPos, java)) return false;
	AABB blockAABB = AABB(blockPos.CastTo<float>(), blockPos.CastTo<float>().add(1.f));
	for (Actor* entity : entityList) {
		if (!entity || !entity->getActorTypeComponent() || !entity->aabbShape) continue;
		if (!entity) continue;
		if (!entity->getActorTypeComponent()) continue;
		if (!entity->aabbShape) continue;
		if (!TargetUtil::isTargetValid(entity, true, false) && entity != localPlayer) continue;
		AABB aabb = entity->aabbShape->aabb;
		if (entity->getActorTypeComponent()->id == 319) {
			aabb = TargetUtil::makeAABB(0.6f, entity->aabbShape->height, entity->getEyePos());
			if (!java) {
				aabb.lower = aabb.lower.sub(0.1f, 0.f, 0.1f);
				aabb.upper = aabb.upper.add(0.1f, 0.f, 0.1f);
			}
		}
		if (aabb.intersects(blockAABB)) return false;
	}
	return true;
}

float AnchorAura::getExplosionDamage(const BlockPos& blockPos, Actor* actor, float exposure) {
	if (!actor || !actor->aabbShape) return 0.0f;
	static constexpr uint8_t explosionSize = 6;
	const AABB& entityAABB = actor->aabbShape->aabb;
	const Vec3<float>& center = entityAABB.getCenter();
	const Vec3<float>& feetPos = Vec3<float>(center.x, entityAABB.lower.y, center.z);
	const float distance = feetPos.dist(blockPos.CastTo<float>()) / explosionSize;
	if (distance <= 1.f) {
		const float impact = (1.f - distance) * exposure;
		const uint16_t damage = std::max((int)(((impact * impact + impact) / 2) * 8 * explosionSize + 1), 0);
		uint8_t armorPoints = 0;
		uint8_t epf = 0;
		for (int i = 0; i < 4; i++) {
			const ItemStack* armor = actor->getArmor(i);
			if (armor->isValid()) {
				armorPoints += armor->item.get()->getArmorValue();
				epf += 5;
			}
		}
		static constexpr uint8_t maxEpf = 25;
		float finalDamage = damage;
		finalDamage -= finalDamage * armorPoints * 0.04f;
		finalDamage -= finalDamage * std::min(std::ceilf(std::min(epf, maxEpf) * 0.5f), 20.f) * 0.04f;
		return std::max(0.f, finalDamage);
	}
	return 0.0f;
}

BlockPos AnchorAura::getPlacePos(Actor* actor, LocalPlayer* localPlayer) {
	if (!actor || !localPlayer) return BlockPos(0, 0, 0);
	Vec3<float> actorPos = actor->getPos().floor().sub(0.f, 1.f, 0.f);
	static std::vector<BlockPos> checkList = { {1, 0, 0}, {-1, 0, 0}, {0, 0, 1}, {0, 0, -1} };
	if (actor == nullptr) return BlockPos(0, 0, 0); 
	BlockPos pos = BlockPos(static_cast<int>(actorPos.x), static_cast<int>(actorPos.y), static_cast<int>(actorPos.z));
	if (isValid(pos.add(0, 2, 0), actor, localPlayer)) return pos.add(0, 2, 0);
	for (int i = 0; i < 2; i++) {
		BlockPos blockPos = pos.add(0, i, 0);
		if (isValid(blockPos, actor, localPlayer) && PlayerUtil::canPlaceBlock(blockPos.add(0, 1, 0), java)) return blockPos;
	}
	for (const BlockPos& block : checkList) {
		BlockPos blockPos = pos.add(block);
		if (isValid(blockPos, actor, localPlayer)) return blockPos;
		if (!isValid(blockPos, actor, localPlayer) && java) {
			BlockPos abovePos = pos.add(0, 2, 0).add(block);
			if (isValid(abovePos, actor, localPlayer) && PlayerUtil::canPlaceBlock(abovePos.sub(0, 1, 0), java)) return abovePos;
		}
		if (faceplace) {
			BlockPos facePos = pos.add(block).add(0, 1, 0);
			if (isValid(facePos, actor, localPlayer)) return facePos;
		}
	}
	if (!hardcode) {
		std::vector<std::pair<BlockPos, float>> placeList;
		for (int x = -proximity; x <= proximity; x++) {
			for (int y = -3; y <= proximity; y++) {
				for (int z = -proximity; z <= proximity; z++) {
					BlockPos blockPos = actorPos.CastTo<int>().add(x, y, z);
					if (blockPos.y != actorPos.y + 1) {
						if (isValid(blockPos, actor, localPlayer)) {
							float exposure = WorldUtil::getSeenPercent(blockPos.CastTo<float>().add(0.5f, 0.f, 0.5f), actor);
							if (WorldUtil::getBlock(blockPos)->blockLegacy->blockId == 527) exposure = 1.f;
							float damage = getExplosionDamage(blockPos, actor, exposure);
							if (damage >= minDamage) placeList.push_back(std::make_pair(blockPos, damage));
						}
					}
				}
			}
		}
		if (!placeList.empty()) {
			std::sort(placeList.begin(), placeList.end(), [&](std::pair<BlockPos, float> p1, std::pair<BlockPos, float> p2) {
				return p1.second > p2.second;
			});
			return placeList[0].first;
		}
	}
	return BlockPos(0, 0, 0);
}

void AnchorAura::onNormalTick(LocalPlayer* localPlayer) {
	static uint8_t oldSlot = -1;
	if (!localPlayer || !localPlayer->gameMode) {
		clearCache();
		return;
	}
	entityList.clear();
	targetList.clear();
	renderList.clear();
	GameMode* gm = localPlayer->gameMode;
	if (!gm) {
		clearCache();
		return;
	}
	if (eatStop && localPlayer->getItemUseDuration() > 0) {
		clearCache();
		return;
	}
	if (!silent && Game::isRightClickDown()) {
		clearCache();
		return;
	}
	for (Actor* actor : localPlayer->level->getRuntimeActorList()) {
		if (!actor) continue;
		entityList.push_back(actor);
	}
	if (entityList.empty()) return; // shouldnt happen but...
	for (Actor* actor : entityList) {
		if (!TargetUtil::isTargetValid(actor, false, true, targetRange)) continue;
		targetList.push_back(actor);
	}
	if (self) targetList.push_back(localPlayer);
	if (targetList.empty()) {
		clearCache();
		return;
	}
	std::sort(targetList.begin(), targetList.end(), TargetUtil::sortByDist);
	if (oldSlot == -1) oldSlot = InventoryUtil::getSelectedSlot();
	BlockPos placePos = getPlacePos(targetList.front(), localPlayer);
	renderList.push_back(placePos);
	if (charged[placePos] && placed[placePos]) {
		static int scan = 0;
		if (scan >= detonateDelay) {
			if (InventoryUtil::getHeldItemId() == 89) InventoryUtil::switchSlot(InventoryUtil::findItemSlotInHotbar(65264));
			if (WorldUtil::getBlock(placePos)->blockLegacy->blockId == 527 && InventoryUtil::getHeldItemId() != 89 && InventoryUtil::getHeldItemId() != 0) {
				if (self) Game::DisplayClientMessage("Detonating..");
				if (gm->buildBlock(placePos, 0, false)) {
					if (self) Game::DisplayClientMessage("Done!");
					placed[placePos] = false;
					charged[placePos] = false;
				}
			}
			if (WorldUtil::getBlock(placePos)->blockLegacy->blockId != 527) {
				placed[placePos] = false;
				charged[placePos] = false;
			}
			scan = 0;
		}
		else scan++;
	}
	else if (!placed[placePos] && !charged[placePos]) {
		static int scan = 0;
		if (scan >= placeDelay) {
			uint8_t anchor = InventoryUtil::findItemSlotInHotbar(65264);
			if (WorldUtil::canBuildOn(placePos)) {
				if (InventoryUtil::getSelectedSlot() != anchor) {
					InventoryUtil::switchSlot(anchor);
					if (java) InventoryUtil::sendMobEquipment(anchor);
				}
				else {
					if (PlayerUtil::tryPlaceBlock(placePos, java, true)) {
						if (self) Game::DisplayClientMessage("Placing..");
						placed[placePos] = true;
					}
				}
			}
			else {
				placed[placePos] = true;
			}
			scan = 0;
		}
		else scan++;
	}
	else if (placed[placePos] && !charged[placePos]) {
		static int scan = 0;
		if (scan >= igniteDelay) {
			uint8_t glowstone = InventoryUtil::findItemSlotInHotbar(89);
			if (WorldUtil::getBlock(placePos)->blockLegacy->blockId == 527 && !charged[placePos]) {
				if (InventoryUtil::getSelectedSlot() != glowstone) {
					InventoryUtil::switchSlot(glowstone);
					if (java) InventoryUtil::sendMobEquipment(glowstone);
				}
				else {
					if (gm->buildBlock(placePos, 0, false)) {
						if (self) Game::DisplayClientMessage("Igniting..");
						charged[placePos] = true;
						scan = 0;
					}
				}
			}
			else {
				placed[placePos] = false; // fix multicharge
				scan = 0;
			}
		}
		else {
			scan++;
		}
	}
	else if (!placed[placePos] && charged[placePos]) {
		if (self) Game::DisplayClientMessage("Attempting to re-sync AnchorAura");
		charged[placePos] = false;
	}
	else {
		if (self) Game::DisplayClientMessage("AnchorAura broke!");
		clearCache();
	}
	if (InventoryUtil::getSelectedSlot() != oldSlot && silent && oldSlot != -1) {
		InventoryUtil::switchSlot(oldSlot);
		if (java) InventoryUtil::sendMobEquipment(oldSlot);
		oldSlot = -1;
	}
}

void AnchorAura::clearCache() {
	for (auto it = placed.begin(); it != placed.end(); ) {
		it->second = false;
		it++;
	}
	for (auto it = charged.begin(); it != charged.end(); ) {
		it->second = false;
		it++;
	}
}

void AnchorAura::onLevelRender() {
	static BlockPos lastRenderPos;
	static float decreasingAlpha = 0.f;
	if (renderList.empty()) {
		decreasingAlpha = Math::lerp(decreasingAlpha, 0.f, D2D::deltaTime * 4.f);
		AABB blockAABB(lastRenderPos.CastTo<float>(), lastRenderPos.CastTo<float>().add(1.f));
		MCR::drawBox3dFilled(blockAABB, UIColor(renderColor.r, renderColor.g, renderColor.b, (uint8_t)((float)renderColor.a * decreasingAlpha)), UIColor(renderColor.r, renderColor.g, renderColor.b, (uint8_t)((float)renderColor.a * decreasingAlpha)));
	}
	else {
		decreasingAlpha = 1.f;
		for (const BlockPos& blockPos : renderList) {
			if (!WorldUtil::getBlock(blockPos)) continue;
			AABB blockAABB = AABB(blockPos.CastTo<float>(), blockPos.CastTo<float>().add(1.f));
			lastRenderPos = blockPos;
			MCR::drawBox3dFilled(blockAABB, renderColor, renderColor);
		}
	}
}

void AnchorAura::onEnable() {
	clearCache();
}

void AnchorAura::onDisable() {
	clearCache();
}