#include "BedAura.h"

BedAura::BedAura() : Module("BedAura", "Places and breaks beds automatically", Category::COMBAT) {
	// Place
	registerSetting(new SliderSetting<int>("Place Dist", "NULL", &placeRange, placeRange, 1, 12));
	registerSetting(new SliderSetting<int>("Proximity", "NULL", &proximity, proximity, 1, 5));
	registerSetting(new SliderSetting<int>("Place Delay", "NULL", &placeDelay, placeDelay, 0, 20));
	// Break
	registerSetting(new SliderSetting<int>("Break Dist", "NULL", &breakRange, breakRange, 0, 5));
	registerSetting(new SliderSetting<int>("Break Delay", "NULL", &breakDelay, breakRange, 0, 20));
	// Logic
	registerSetting(new EnumSetting("Priority", "Targetting priority", { "Distance", "Damage" }, &priority, 0));
	registerSetting(new BoolSetting("Extrapolate", "Predicts enemy movement", &extrapolation, false));
	registerSetting(new SliderSetting<int>("Extrapolate Ticks", "NULL", &extrapolateAmt, extrapolateAmt, 1, 5, [&]() -> bool {
		return extrapolation;
		}));
	registerSetting(new SliderSetting<int>("Target Dist", "NULL", &targetRange, targetRange, 0, 20));
	registerSetting(new SliderSetting<float>("Enemy Dmg", "Minimum damage that the bedaura should deal to enemy", &targetDamage, targetDamage, 0.f, 36.f));
	registerSetting(new SliderSetting<float>("Self Dmg", "Maximum damage that the bedaura should deal to you, autocrystal ignores this is safety is off", &localDamage, localDamage, 0.f, 36.f));
	registerSetting(new BoolSetting("EatStop", "Stops bedaura when eating", &eatStop, true));
	registerSetting(new BoolSetting("Phase Place", "In IGNPVP, when an entity is phased vertically or phased far enough horizontally, explosions can go through blocks to deal full damage to him\nthis will allow the bedaura to place on places that would normally deal zero damage", &phasePlace, true));
	registerSetting(new BoolSetting("Container Check", "Wont place when theres a container below", &containerCheck, true));
	registerSetting(new BoolSetting("Swap", "Switches to bed", &swap, true));
	registerSetting(new BoolSetting("SwapBack", "Switches back to old slot for silent switching", &swapBack, true));
	registerSetting(new BoolSetting("Self Test", "Test on self", &selfTest, false));
	// Terrain
	registerSetting(new BoolSetting("Terrain", "Places support blocks", &terrain, false));
	registerSetting(new BoolSetting("Packet Place", "Uses packets to place the support blocks", &packet, false, [&]() -> bool {
		return terrain;
		}));
	registerSetting(new SliderSetting<float>("Damage", "NULL", &minTerrain, minTerrain, 0.f, 20.f));
	registerSetting(new SliderSetting<int>("Delay", "NULL", &terrainDelay, 1, 0, 20, [&]() -> bool {
		return terrain;
		}));
	registerSetting(new SliderSetting<int>("Terrain Proximity", "NULL", &terrainProximity, 1, 1, 6, [&]() -> bool {
		return terrain;
		}));
	// Safety
	registerSetting(new BoolSetting("Safety", "Very smart safety that tries its best to not kill u", &safety, false));
	registerSetting(new SliderSetting<float>("Ratio", "Safety Ratio, if the difference of best damage placement's enemy damage and the best safe placement enemy damage is less than this then it will place the best safe placement instead", &dmgDifference, dmgDifference, 0.f, 5.f, [&]() -> bool {
		return safety;
		}));
	registerSetting(new SliderSetting<float>("Danger Damage", "If the damage that will be dealt to u is gonna be more than this then it will force the client to use the safe placement", &dangerDamage, dangerDamage, 0.f, 12.f, [&]() -> bool {
		return safety;
		}));
	// Render
	registerSetting(new EnumSetting("Render", "Renders where we are placing", { "Off", "Box", "Flat" }, &renderType, 2));
	registerSetting(new BoolSetting("Render Damage", "NULL", &dmgText, false, [&]() -> bool {
		return renderType != 0;
		}));
	registerSetting(new BoolSetting("Fade", "Fade render!!", &fade, true, [&]() -> bool {
		return renderType != 0;
		}));
	registerSetting(new SliderSetting<float>("Fade Duration", "Fading duration in seconds", &fadeDuration, 1.f, 0.1f, 3.f, [&]() -> bool {
		return renderType != 0 && fade;
		}));
	registerSetting(new BoolSetting("Slide", "Slide renders", &slide, false, [&]() -> bool {
		return renderType != 0;
		}));
	registerSetting(new SliderSetting<float>("Slide Speed", "Sliding speed", &slideSpeed, 10.f, 0.f, 20.f, [&]() -> bool {
		return renderType != 0 && slide;
		}));
	registerSetting(new ColorSetting("Fill Color", "Color of the render", &renderColor, UIColor(255, 255, 255), true, true, [&]() -> bool {
		return renderType != 0;
		}));
	registerSetting(new ColorSetting("Line Color", "Color of the render", &lineColor, UIColor(255, 255, 255), true, true, [&]() -> bool {
		return renderType != 0;
		}));
	registerSetting(new ColorSetting("Text Color", "Color of the render", &damageColor, UIColor(255, 255, 255), false, true, [&]() -> bool {
		return renderType != 0 && dmgText;
		}));
}

float BedAura::getExposure(const BlockPos& blockPos, Actor* actor) {
	//if (WorldUtil::getBlock(blockPos)->blockLegacy->blockId == 26) return 1.f;
	const Vec3<float>& eyePos = actor->getPos();
	const Vec3<float>& floorPos = actor->getPos().floor();
	const Vec3<float>& actorPos = floorPos.sub(0.f, 1.f, 0.f);
	if (phasePlace) {
		const AABB& actorAABB = actor->aabbShape->aabb;
		const AABB& blockAABB = AABB(actorPos.sub(0, 1, 0), actorPos.sub(0, 1, 0).add(1.f));
		if (actorAABB.intersects(blockAABB)) {
			Block* block = WorldUtil::getBlock(blockPos.CastTo<int>());
			if (block->blockLegacy->isSolid()) return 1.f;
		}
	}
	const Vec3<float>& sub = eyePos - floorPos;
	const float realX = sub.x - 0.5f;
	const float realZ = sub.z - 0.5f;
	if (0.21 <= realX && 0.21 <= realZ && blockPos == actorPos.add(1, 0, 1).CastTo<int>()) {
		Block* xB = WorldUtil::getBlock(actorPos.add(1, 0, 0).CastTo<int>());
		Block* zB = WorldUtil::getBlock(actorPos.add(0, 0, 1).CastTo<int>());
		if (xB->blockLegacy->isSolid() && zB->blockLegacy->isSolid()) return 0.f;
	}

	return WorldUtil::getExposure(blockPos.CastTo<float>(), actor->aabbShape->aabb, 26);
}

bool BedAura::isBlockValid(const BlockPos& blockPos, Actor* actor) {
	static const std::unordered_set<int> badBlocks = {26, 55, 76, 143, 399, 396, 398, 395, 397, 77, 515, 516, 30, 90};
	const Vec3<float>& actorPos = actor->getPos().floor().sub(0, 1, 0);
	if (blockPos == BlockPos(0, 0, 0)) return false;
	if (Game::getLocalPlayer()->getEyePos().dist(blockPos.CastTo<float>()) > placeRange) return false;
	Block* block = WorldUtil::getBlock(blockPos);
	Block* blockBelow = WorldUtil::getBlock(blockPos.sub(0, 1, 0));
	if (!WorldUtil::canBuildOn(blockPos) && block->blockLegacy->blockId != 26) return false;
	if (WorldUtil::canBuildOn(blockPos.sub(0, 1, 0)) || (WorldUtil::isContainer(blockPos.sub(0, 1, 0)) && containerCheck)) return false;
	if (badBlocks.find(blockBelow->blockLegacy->blockId) != badBlocks.end()) return false;
	if (blockPos.y > actorPos.y) return false;
	if (blockPos.y < actorPos.y && !WorldUtil::canBuildOn(actorPos.sub(0, 1, 0).CastTo<int>())) return false;
	return true;
}

bool BedAura::isTerrainValid(const BlockPos& blockPos, Actor* actor) {
	if (actor->getPos().dist(Game::getLocalPlayer()->getPos()) > placeRange) return false;
	if (!WorldUtil::canBuildOn(blockPos)) return false;
	const AABB blockAABB(blockPos.sub(0, 1, 0).CastTo<float>(), blockPos.sub(0, 1, 0).CastTo<float>().add(1.f));
	for (Actor* entity : Game::getLocalPlayer()->level->getRuntimeActorList()) {
		if (!entity) continue;
		if (!TargetUtil::isTargetValid(entity, true, false) && entity != Game::getLocalPlayer()) continue;
		AABB entityAABB = entity->aabbShape->aabb;
		if (entity->getActorTypeComponent()->id == 319) {
			entityAABB.lower = entityAABB.lower.sub(0.1f, 1.f, 0.1f);
			entityAABB.upper = entityAABB.upper.add(0.1f, 1.f, 0.1f);
		}
		if (entityAABB.intersects(blockAABB)) return false;
	}
	return true;
}

BlockPos BedAura::getHeadPosTerrain(const BlockPos& bedPos, Actor* actor) {
	BlockPos checkpos = bedPos;
	const Vec3<float>& actorPos = actor->getPos().floor().sub(0, 1, 0);
	static const BlockPos checkList[4] = { {1, 0, 0}, {-1, 0, 0}, {0, 0, 1}, {0, 0, -1} };
	for (const BlockPos& check : checkList) {
		const BlockPos& pos = bedPos.add(check).floor();
		if ((isTerrainValid(pos, actor) || isBlockValid(pos, actor)) && (pos.x != actorPos.x || pos.z != actorPos.z)) {
			checkpos = pos;
			break;
		}
	}
	return checkpos;
}

BlockPos BedAura::getHeadPos(const BlockPos& bedPos, Actor* actor) {
	BlockPos checkpos = bedPos;
	const Vec3<float>& actorPos = actor->getPos().floor().sub(0, 1, 0);
	static const BlockPos checkList[4] = { {1, 0, 0}, {-1, 0, 0}, {0, 0, 1}, {0, 0, -1} };
	for (const BlockPos& check : checkList) {
		const BlockPos& pos = bedPos.add(check).floor();
		if (isBlockValid(pos, actor) && (pos.x != actorPos.x || pos.z != actorPos.z)) {
			checkpos = pos;
			break;
		}
	}
	return checkpos;
}


void BedAura::getTerrainList(Actor* actor) {
	const int& radius = terrainProximity;
	const Vec3<float>& actorPos = Vec3<float>(actor->getPos().x, ceil(actor->aabbShape->aabb.lower.y), actor->getPos().z);
	for (int x = -radius; x <= radius; x++) {
		for (int y = -radius; y < radius; y++) {
			for (int z = -radius; z <= radius; z++) {
				const BlockPos blockPos((int)actorPos.x + x, (int)actorPos.y + y, (int)actorPos.z + z);
				const bool& isValid = isTerrainValid(blockPos, actor);
				if (isValid) {
					const BlockPos& headPos = getHeadPosTerrain(blockPos, actor);
					const Terrain terrain(blockPos, headPos, actor, getExposure(blockPos, actor));
					if (terrain.targetDamage >= minTerrain && headPos != blockPos) terrainList.emplace_back(terrain);
				}
			}
		}
	}
	std::sort(terrainList.begin(), terrainList.end(), sortByDamage);
}

void BedAura::placeObsidian(GameMode* gm) {
	if (terrainList.empty()) return;
	static float iDelay = 0;
	if (InventoryUtil::getHeldItemId() != 49) return;
	if (TimerUtil::hasReached<ticks>(&iDelay, terrainDelay)) {
		if (!PlayerUtil::tryPlaceBlock(terrainList[0].placePos, false, packet) && WorldUtil::canBuildOn(terrainList[0].placePos)) PlayerUtil::tryPlaceBlockSupport(terrainList[0].placePos, terrainProximity, false, packet);
		PlayerUtil::tryPlaceBlock(terrainList[0].headPos, false, packet);
	}
}

void BedAura::onTerrain(LocalPlayer* localPlayer, Actor* actor) {
	if (!placeList.empty()) {
		terrainList.clear();
		return;
	}
	getTerrainList(actor);
	uint8_t oldSlot = InventoryUtil::getSelectedSlot();
	InventoryUtil::switchSlot(InventoryUtil::findItemSlotInHotbar(49));
	placeObsidian(localPlayer->gameMode);
	if (swapBack) InventoryUtil::switchSlot(oldSlot);
}

void BedAura::generatePlacement(LocalPlayer* localPlayer, Actor* actor) {
	for (int x = -proximity; x <= proximity; x++) {
		for (int y = -5; y <= 3; y++) {
			for (int z = -proximity; z <= proximity; z++) {
				const Vec3<float>& actorPos = actor->getPos().floor().sub(0, 1, 0);
				const Vec3<float>& velo = actor->stateVector->velocity;
				const Vec3<float>& addedPos = Vec3<float>(extrapolation ? (velo.x * extrapolateAmt, velo.y * extrapolateAmt, velo.z * extrapolateAmt) : (0.f, 0.f, 0.f));
				const BlockPos& blockPos = BlockPos((int)actorPos.x + (int)addedPos.x + x, (int)actorPos.y + (int)addedPos.y + y, (int)actorPos.z + (int)addedPos.z + z);
				if (isBlockValid(blockPos, actor)) {
					const Place placement(localPlayer, blockPos, actor, getExposure(blockPos, actor));
					const BlockPos& headPos = getHeadPos(blockPos, actor);
					if (placement.localDamage < localDamage && placement.targetDamage >= targetDamage && placement.placePos != headPos) placeList.emplace_back(placement);
				}
			}
		}
	} std::sort(placeList.begin(), placeList.end(), BedAura::sortByDamage);
}

void BedAura::sortPlacement(LocalPlayer* localPlayer, Actor* actor) {
	static const BlockPos checkList[4] = { {1, 0, 0}, {-1, 0, 0}, {0, 0, 1}, {0, 0, -1} };
	const BlockPos& actorPos = actor->getPos().floor().sub(0, 1, 0).CastTo<int>();
	placeCheck = true;
	if (isBlockValid(actorPos, actor)) {
		const BlockPos& headPos = getHeadPos(actorPos, actor);
		const Place placement(localPlayer, actorPos, actor, getExposure(actorPos, actor));
		if (placement.placePos != headPos) placeList.push_back(placement);
	}
	if (!placeCheck) return;
	if (placeList.empty()) return;
	if (safety) {
		for (const Place& placement : placeList) {
			if (placeList[0].placePos == placement.placePos) continue;
			if (placeList[0].localDamage >= dangerDamage) {
				if ((placement.targetDamage >= placeList[0].targetDamage || placeList[0].targetDamage - placement.targetDamage <= dmgDifference) && placement.localDamage <= placeList[0].localDamage) placeList[0] = placement;
			}
		}
	}
	for (const Place& placement : placeList) {
		BlockPos placePos = placement.placePos;
		if (isBlockValid(placePos, actor)) {
			BlockPos headPos = getHeadPos(placePos, actor);
			if (placePos != headPos && setYaw(placePos, headPos)) {
				placeBed(placePos);
				explodeBed(placePos);
				placeCheck = false;
				break;
			}
		}
	}
}

void BedAura::placeBed(const BlockPos& blockPos) {
	if (InventoryUtil::getHeldItemId() != 26 && InventoryUtil::getHeldItemId() != 355) return;
	LocalPlayer* lp = Game::getLocalPlayer();
	static int placeIncr = 0;
	if (placeIncr >= placeDelay) {
		for (int i = 0; i < 3; i++) {
			const BlockPos& placePos = blockPos.add(0, -1, 0);
			lp->gameMode->buildBlock(placePos, 1, false);
		}
		placeIncr = 0;
	}
	else placeIncr++;
}

void BedAura::explodeBed(const BlockPos& blockPos) {
	static int explodeIncr = 0;
	if (InventoryUtil::getHeldItemId() != 26 && InventoryUtil::getHeldItemId() != 355) return;
	LocalPlayer* lp = Game::getLocalPlayer();
	if (explodeIncr >= breakDelay) {
		lp->gameMode->buildBlock(blockPos, 1, false);
		Vec3<float> playerPos = lp->getPos();
		for (int x = (int)playerPos.x - breakRange; x < playerPos.x + breakRange; x++) {
			for (int z = (int)playerPos.z - breakRange; z < playerPos.z + breakRange; z++) {
				for (int y = (int)playerPos.y - breakRange; y < playerPos.y + breakRange; y++) {
					const BlockPos& bedPos = BlockPos(x, y, z);
					if (WorldUtil::getBlock(bedPos)->blockLegacy->blockId == 26) lp->gameMode->buildBlock(bedPos, 1, false);		
				}
			}
		}
		explodeIncr = 0;
	}
	else explodeIncr++;
}

void BedAura::onNormalTick(LocalPlayer* localPlayer) {
	terrainList.clear();
	placeList.clear();
	targetList.clear();
	extrapolateRender.clear();
	if (eatStop && localPlayer->getItemUseDuration() > 0) return;
	for (Actor* actor : localPlayer->level->getRuntimeActorList()) {
		if (!TargetUtil::isTargetValid(actor)) continue;
		if (actor->getEyePos().dist(localPlayer->getEyePos()) > targetRange) continue;
		targetList.push_back(actor);
	}
	if (selfTest) targetList.push_back(localPlayer);
	if (targetList.empty()) return;
	std::sort(targetList.begin(), targetList.end(), TargetUtil::sortByDist);
	const int oldSlot = InventoryUtil::getSelectedSlot();
	if (swap) InventoryUtil::switchSlot(InventoryUtil::findItemSlotInHotbar(InventoryUtil::hasItem(26) ? 26 : 355));
	for (Actor* actor : targetList) {
		generatePlacement(localPlayer, actor);
		sortPlacement(localPlayer, actor);
		if (terrain) onTerrain(localPlayer, actor);
		if (priority != 1) break;
	}
	if (swapBack) InventoryUtil::switchSlot(oldSlot);
}

void BedAura::onSendPacket(Packet* packet, bool& cancel) {
	if (placeList.empty()) return;
	if (packet->getId() == PacketID::PlayerAuthInput) {
		PlayerAuthInputPacket* authPkt = reinterpret_cast<PlayerAuthInputPacket*>(packet);
		authPkt->headYaw = static_cast<float>(realYaw);
		authPkt->rotation.y = static_cast<float>(realYaw);
	}
	if (packet->getId() == PacketID::MovePlayer) {
		MovePlayerPacket* movePkt = reinterpret_cast<MovePlayerPacket*>(packet);
		movePkt->headYaw = static_cast<float>(realYaw);
	}
}

void BedAura::onLevelRender() {
	static int renderxz1, renderxz2, renderxz3, renderxz4;
	static Vec3<float> lastPos;
	static std::vector<Fade> fadeList;
	static float decreasingAlpha = 1.f;
	if (renderType == 0) return;
	if (realYaw == -90 || realYaw == 180) {
		renderxz1 = (realYaw == -90) ? 0 : 0;
		renderxz2 = (realYaw == -90) ? 0 : -1;
		renderxz3 = (realYaw == -90) ? 2 : 1;
		renderxz4 = (realYaw == -90) ? 1 : 1;
	}
	else if (realYaw == 90 || realYaw == 0) {
		renderxz1 = (realYaw == 90) ? -1 : 0;
		renderxz2 = (realYaw == 90) ? 0 : 0;
		renderxz3 = (realYaw == 90) ? 1 : 1;
		renderxz4 = (realYaw == 90) ? 1 : 2;
	}
	for (auto it = fadeList.begin(); it != fadeList.end(); ) {
		it->deltaTime += D2D::deltaTime;
		const float fadeAlpha = 1.f - (it->deltaTime / it->duration);
		if (fadeAlpha <= 0.0f) it = fadeList.erase(it);
		else {
			AABB bedAABB = AABB(it->fadePos.add(static_cast<float>(renderxz1), 0.f, static_cast<float>(renderxz2)), it->fadePos.add(static_cast<float>(renderxz3), renderType == 2 ? 0.f : 0.5f, static_cast<float>(renderxz4)));
			MCR::drawBox3dFilled(bedAABB, UIColor(renderColor.r, renderColor.g, renderColor.b, renderColor.a * fadeAlpha), UIColor(lineColor.r, lineColor.g, lineColor.b, lineColor.a * fadeAlpha));
			it++;
		}
	}
	if (placeList.empty()) {
		AABB bedAABB = AABB(lastPos.add(static_cast<float>(renderxz1), 0.f, static_cast<float>(renderxz2)), lastPos.add(static_cast<float>(renderxz3), renderType == 2 ? 0.f : 0.5f, static_cast<float>(renderxz4)));
		decreasingAlpha = Math::lerp(0.f, decreasingAlpha, D2D::deltaTime * 5.f);
		MCR::drawBox3dFilled(bedAABB, UIColor(renderColor.r, renderColor.g, renderColor.b, renderColor.a * decreasingAlpha), UIColor(lineColor.r, lineColor.g, lineColor.b, lineColor.a * decreasingAlpha));
	}
	else {
		decreasingAlpha = 1.f;
		Vec3<float> renderPos = placeList[0].placePos.CastTo<float>();
		if (fade && renderPos != lastPos) fadeList.emplace_back(Fade(lastPos, fadeDuration));
		const float animSpeed = D2D::deltaTime * slideSpeed;
		if (slide) renderPos = lastPos.lerp(renderPos, animSpeed, animSpeed, animSpeed);
		else lastPos = renderPos;
		AABB bedAABB = AABB(renderPos.add(static_cast<float>(renderxz1), 0.f, static_cast<float>(renderxz2)), renderPos.add(static_cast<float>(renderxz3), renderType == 2 ? 0.f : 0.5f, static_cast<float>(renderxz4)));
		MCR::drawBox3dFilled(bedAABB, renderColor, lineColor);
		lastPos = renderPos;
	}
}

void BedAura::onMCRender(MinecraftUIRenderContext* renderCtx) {
	if (renderType == 0 || !dmgText) return;
	if (placeList.empty()) return;
	Vec3<float> renderPos = placeList[0].placePos.CastTo<float>();
	const int& damage = placeList[0].targetDamage;
	MCR::drawTextInWorld(renderPos.sub(0.f, 0.5f, 0.f), std::to_string(damage), damageColor, 0.7f);
}