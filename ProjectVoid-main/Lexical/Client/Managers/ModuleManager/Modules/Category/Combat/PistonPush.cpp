#include "PistonPush.h"

PistonPush::PistonPush() : Module("PistonPush", "Pushes piston into enemy to push them out of hole", Category::COMBAT) {
	registerSetting(new SliderSetting<float>("Target Range", "NULL", &targetRange, targetRange, 1.f, 12.f));
	registerSetting(new SliderSetting<int>("Piston Delay", "NULL", &pistonDelay, pistonDelay, 0, 20));
	registerSetting(new SliderSetting<int>("Redstone Delay", "NULL", &redstoneDelay, redstoneDelay, 0, 20));
	registerSetting(new BoolSetting("Airplace", "NULL", &airplace, airplace));
	registerSetting(new BoolSetting("Packet Place", "NULL", &packetPlace, packetPlace));
	registerSetting(new BoolSetting("Java", "NULL", &java, java));
	registerSetting(new BoolSetting("Self", "NULL", &self, self));
	registerSetting(new ColorSetting("Piston Color", "NULL", &pistonColor, pistonColor));
	registerSetting(new ColorSetting("Redstone Color", "NULL", &redstoneColor, redstoneColor));
}

bool PistonPush::canPlaceBlock(const BlockPos& blockPos, bool airplace, const int targetBlockId) {
	LocalPlayer* localPlayer = Game::getLocalPlayer();
	BlockSource* region = Game::clientInstance->getRegion();
	Block* block = region->getBlock(blockPos);
	BlockLegacy* blockLegacy = block->blockLegacy;
	if (blockLegacy->canBeBuiltOver(region, blockPos)) {
		if (airplace && targetBlockId != 76) return true;
		BlockPos blockToPlace = blockPos;
		static BlockPos checklist[6] = { BlockPos(0, -1, 0),
										 BlockPos(0, 1, 0),
										 BlockPos(0, 0, -1),
										 BlockPos(0, 0, 1),
										 BlockPos(-1, 0, 0),
										 BlockPos(1, 0, 0) };
		for (const BlockPos& current : checklist) {
			BlockPos calc = blockToPlace.sub(current);
			if (targetBlockId == 76) {
				if (!region->getBlock(calc)->blockLegacy->canBeBuiltOver(region, calc) && !WorldUtil::isContainer(calc) && region->getBlock(calc)->blockLegacy->blockId != 33 && region->getBlock(calc)->blockLegacy->blockId != 29) return true;
			}
			else {
				if (!region->getBlock(calc)->blockLegacy->canBeBuiltOver(region, calc) && !WorldUtil::isContainer(calc)) return true;
			}
		}
	}
	return false;
}

bool PistonPush::placeBlock(const BlockPos& blockPos, bool airplace, bool packetPlace) {
	LocalPlayer* localPlayer = Game::getLocalPlayer();
	BlockSource* region = Game::clientInstance->getRegion();
	Block* block = region->getBlock(blockPos);
	BlockLegacy* blockLegacy = block->blockLegacy;
	int heldId = InventoryUtil::getHeldItemId();
	if (blockLegacy->canBeBuiltOver(region, blockPos)) {

		BlockPos blockToPlace = blockPos;

		static BlockPos checklist[6] = { BlockPos(0, -1, 0),
										 BlockPos(0, 1, 0),
										 BlockPos(0, 0, -1),
										 BlockPos(0, 0, 1),
										 BlockPos(-1, 0, 0),
										 BlockPos(1, 0, 0) };

		bool foundCandidate = false;
		uint8_t i = 0;

		for (const BlockPos& current : checklist) {
			BlockPos calc = blockToPlace.sub(current);
			if (heldId != 76) {
				if (!region->getBlock(calc)->blockLegacy->canBeBuiltOver(region, calc) && !WorldUtil::isContainer(calc) && region->getBlock(calc)->blockLegacy->blockId != 143) {
					foundCandidate = true;
					blockToPlace = calc;
					break;
				}
			}
			else {
				if (!region->getBlock(calc)->blockLegacy->canBeBuiltOver(region, calc) && !WorldUtil::isContainer(calc) && region->getBlock(calc)->blockLegacy->blockId != 143 && region->getBlock(calc)->blockLegacy->blockId != 33 && region->getBlock(calc)->blockLegacy->blockId != 29) {
					foundCandidate = true;
					blockToPlace = calc;
					break;
				}
			}
			i++;
		}
		if (heldId != 76) {
			if (airplace && !foundCandidate) {
				foundCandidate = true;
				blockToPlace = blockPos;
				i = 0;
			}
		}
		if (foundCandidate) {
			if (!packetPlace) {
				localPlayer->gameMode->buildBlock(blockToPlace, i, false);
			}
			else {
				PlayerInventory* plrInv = localPlayer->playerInventory;
				std::unique_ptr<ItemUseInventoryTransaction> itemUseInvTransac = ItemUseInventoryTransaction::make_unique();
				itemUseInvTransac->actionType = ItemUseInventoryTransaction::ActionType::Place;
				itemUseInvTransac->blockPos = blockToPlace;
				itemUseInvTransac->targetBlockRuntimeId = *region->getBlock(blockToPlace)->getRuntimeId();
				itemUseInvTransac->face = i;
				itemUseInvTransac->slot = plrInv->selectedSlot;
				itemUseInvTransac->itemInHand = NetworkItemStackDescriptor(plrInv->container->getItem(plrInv->selectedSlot));
				itemUseInvTransac->playerPos = localPlayer->getPos();
				itemUseInvTransac->clickPos = Vec3<float>(0.5f, 0.5f, 0.5f);

				InventoryTransactionPacket pk(std::move(itemUseInvTransac));
				localPlayer->level->getPacketSender()->send(&pk);
			}
			return true;
		}
	}
	return false;
}

bool PistonPush::isPlaceValid(const BlockPos& blockPos, const int targetBlockId) {
	if (WorldUtil::getBlock(blockPos)->blockLegacy->blockId == targetBlockId) return true;
	if (!canPlaceBlock(blockPos, airplace, targetBlockId)) return false;
	if (targetBlockId != 76) {
		AABB blockAABB = AABB(blockPos.CastTo<float>(), blockPos.CastTo<float>().add(1.f));
		for (Actor* actor : actorList) {
			if (!actor) continue;
			if (!actor->aabbShape) continue;
			AABB actorAABB = actor->aabbShape->aabb;
			if (actor->getActorTypeComponent()->id == 319) actorAABB = TargetUtil::makeAABB(0.6f, actor->aabbShape->height, actor->getEyePos());
			if (blockAABB.intersects(actorAABB)) return false;
		}
	}
	return true;
}

BlockPos PistonPush::getOffset(LocalPlayer* localPlayer) {
	float rotation = localPlayer->rotation->presentRot.y;
	if (rotation < 0) rotation += 360.0;
	if ((0 <= rotation && rotation < 45) || (315 <= rotation && rotation < 360)) return BlockPos(0, 0, 1);
	else if (45 <= rotation && rotation < 135) return BlockPos(-1, 0, 0);
	else if (135 <= rotation && rotation < 225) return BlockPos(0, 0, -1);
	else if (225 <= rotation && rotation < 315) return BlockPos(1, 0, 0);
	return BlockPos(0, 0, 0);
}

std::pair<BlockPos, BlockPos> PistonPush::getPlacePos(Actor& actor) {
	static std::vector<BlockPos> redstoneList;
	RedstoneType redstoneType = getRedstoneType();
	redstoneList.clear();
	static std::vector<BlockPos> checkList = {
		{1, 0, 0}, {-1, 0, 0}, {0, 0, 1}, {0, 0, -1}
	};
	Vec3<float> actorPos = actor.getPos().floor().sub(0, 1, 0);
	BlockPos piston = BlockPos(0, 0, 0);
	BlockPos redstone = BlockPos(0, 0, 0);
	BlockPos offset = getOffset(Game::getLocalPlayer());
	for (int y : {0, 1}) {
		BlockPos pos = actorPos.CastTo<int>().add(offset.x, y, offset.z);
		if (isPlaceValid(pos, 33) || isPlaceValid(pos, 29)) {
			piston = pos;
			break;
		}
	}
	for (const BlockPos& check : checkList) {
		redstoneList.push_back(check);
	}
	if (java) { // quassi connectivity(idek if i spelled that right)
		for (const BlockPos& check : checkList) {
			BlockPos pos = piston.add(check);
			if (!WorldUtil::canBuildOn(pos) && isPlaceValid(pos.add(0, 1, 0), redstoneType == RedstoneType::RedstoneBlock ? 153 : 76)) redstoneList.push_back(BlockPos(check.x, 1, check.z));
		}
	}
	for (const BlockPos& check : redstoneList) {
		BlockPos pos = piston.add(check);
		if (isPlaceValid(pos, redstoneType == RedstoneType::RedstoneBlock ? 153 : 76)) {
			redstone = pos;
			break;
		}
	}
	return std::make_pair(piston, redstone);
}
PistonPush::RedstoneType PistonPush::getRedstoneType() {
	if (InventoryUtil::hasItem(153)) return RedstoneType::RedstoneBlock;
	else if (InventoryUtil::hasItem(76)) return RedstoneType::RedstoneTorch;
	return RedstoneType::None;
}
void PistonPush::onNormalTick(LocalPlayer* localPlayer) {
	static int lastPiston = 0;
	static int lastRedstone = 0;
	targetList.clear();
	actorList.clear();
	if (localPlayer->getItemUseDuration() > 0) return;
	if (getRedstoneType() == RedstoneType::None) return;
	for (Actor* actor : localPlayer->level->getRuntimeActorList()) {
		if (!actor) continue;
		if (!TargetUtil::isTargetValid(actor, true, false) && actor != localPlayer) continue;
		actorList.push_back(actor);
	}
	if (actorList.empty()) return;
	for (Actor* actor : actorList) {
		if (!actor) continue;
		if (!TargetUtil::isTargetValid(actor, false, true, targetRange)) continue;
		targetList.push_back(actor);
	}
	if (self) targetList.push_back(localPlayer);
	if (targetList.empty()) return;
	std::sort(targetList.begin(), targetList.end(), TargetUtil::sortByDist);
	Actor& mainActor = *targetList.front();
	std::pair<BlockPos, BlockPos> placePair = getPlacePos(mainActor);
	if (placePair.first == BlockPos(0, 0, 0) || placePair.second == BlockPos(0, 0, 0)) return;
	auto getPiston = []() -> uint8_t {
		uint8_t slot = -1;
		for (uint8_t i = 0; i < 9; i++) {
			if (!InventoryUtil::isValid(InventoryUtil::getItem(i))) continue;
			if (InventoryUtil::getItemId(InventoryUtil::getItem(i)) == 29 or InventoryUtil::getItemId(InventoryUtil::getItem(i)) == 33) {
				slot = i;
				break;
			}
		}
		return slot;
	};
	uint8_t oldSlot = InventoryUtil::getSelectedSlot();
	if (lastPiston >= pistonDelay) {
		InventoryUtil::switchSlot(getPiston());
		if (java) InventoryUtil::sendMobEquipment(getPiston());
		localPlayer->rotation->presentRot.x = 0;
		placeBlock(placePair.first, airplace, packetPlace);
		lastPiston = 0;
	}
	else lastPiston++;
	if (lastRedstone >= redstoneDelay) {
		InventoryUtil::switchSlot(InventoryUtil::findItemSlotInHotbar(getRedstoneType() == RedstoneType::RedstoneBlock ? 152 : 76));
		if (java) InventoryUtil::sendMobEquipment(InventoryUtil::findItemSlotInHotbar(getRedstoneType() == RedstoneType::RedstoneBlock ? 152 : 76));
		placeBlock(placePair.second, airplace, packetPlace);
		lastRedstone = 0;
	}
	else lastRedstone++;
	if (InventoryUtil::getSelectedSlot() != oldSlot) InventoryUtil::switchSlot(oldSlot);
	if (java && PlayerUtil::selectedSlotServerSide != oldSlot) InventoryUtil::sendMobEquipment(oldSlot);
}

void PistonPush::onLevelRender() {
	if (targetList.empty()) return;
	Actor& mainActor = *targetList.front();
	std::pair<BlockPos, BlockPos> placePair = getPlacePos(mainActor);
	if (placePair.first == BlockPos(0, 0, 0) || placePair.second == BlockPos(0, 0, 0)) return;
	AABB pistonAABB = AABB(placePair.first.CastTo<float>(), placePair.first.CastTo<float>().add(1.f));
	AABB redstoneAABB = AABB(placePair.second.CastTo<float>(), placePair.second.CastTo<float>().add(1.f));
	MCR::drawBox3dFilled(pistonAABB, pistonColor, pistonColor);
	MCR::drawBox3dFilled(redstoneAABB, redstoneColor, redstoneColor);
}