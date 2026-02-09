#include "SpiderTower.h"

SpiderTower::SpiderTower() : Module("SpiderTower", "Builds blocks in front of u and spider up to escape!", Category::MISC) {
	registerSetting(new SliderSetting<float>("Spider Speed", "Speed of spidering", &spiderSpeed, 0.35f, 0.1f, 1.f));
	registerSetting(new SliderSetting<int>("Height", "Height of the blocks", &height, 3, 1, 10));
	registerSetting(new SliderSetting<int>("Blocks Per Tick", "NULL", &bpt, 4, 1, 10));
	registerSetting(new BoolSetting("EChest Below", "Places echest below you so they cant chase you with spider too", &chestBelow, false));
	registerSetting(new BoolSetting("Packet Place", "Places using packets", &packetPlace, false));
	registerSetting(new BoolSetting("MobEq", "Sends mobeq packets", &mobEq, false));
}

BlockPos SpiderTower::getOffset(LocalPlayer* localPlayer) {
	float rotation = localPlayer->rotation->presentRot.y;
	if (rotation < 0) rotation += 360.0;
	if ((0 <= rotation && rotation < 45) || (315 <= rotation && rotation < 360)) return BlockPos(0, 0, 1);
	else if (45 <= rotation && rotation < 135) return BlockPos(-1, 0, 0);
	else if (135 <= rotation && rotation < 225) return BlockPos(0, 0, -1);
	else if (225 <= rotation && rotation < 315) return BlockPos(1, 0, 0);
	return BlockPos(1, 0, 0);
}

void SpiderTower::onNormalTick(LocalPlayer* localPlayer) {
	static int delay = 0;
	const uint8_t oldSlot = InventoryUtil::getSelectedSlot();
	BlockPos offset = getOffset(localPlayer);
	static BlockPos offsets[4] = { {1, 0, 0}, {-1, 0, 0}, {0, 0, 1}, {0, 0, -1} };
	static bool wasHorizontalCollision = false;
	if (localPlayer->hasHorizontalCollision()) {
		localPlayer->stateVector->velocity.y = spiderSpeed;
		wasHorizontalCollision = true;
	}
	else {
		if (wasHorizontalCollision) {
			localPlayer->stateVector->velocity.y = 0.f;
			wasHorizontalCollision = false;
		}
	}
	InventoryUtil::switchSlot(InventoryUtil::findItemSlotInHotbar(49));
	if (mobEq) InventoryUtil::sendMobEquipment(InventoryUtil::findItemSlotInHotbar(49));
	int tickCD = 0;
	for (int i = 0; i < height; i++) {
		const Vec3<float>& humanPos = localPlayer->getPos().floor().sub(0.f, 1.f, 0.f);
		const BlockPos& pos = humanPos.CastTo<int>().add(offset).add(0, i, 0);
		if (!PlayerUtil::canPlaceBlock(pos)) continue;
		if (InventoryUtil::getHeldItemId() == 49) PlayerUtil::tryPlaceBlock(pos, false, false);
		if (++tickCD >= bpt) break;
	}
	if (chestBelow) {
		if (delay >= 20) {
			const Vec3<float>& humanPos = localPlayer->getPos().floor().sub(0.f, 1.f, 0.f);
			for (const BlockPos& check : offsets) {
				const BlockPos& pos = humanPos.CastTo<int>().add(offset).add(check).add(0, -1, 0);
				if (PlayerUtil::canPlaceBlock(pos)) {
					InventoryUtil::switchSlot(InventoryUtil::findItemSlotInHotbar(130));
					if (InventoryUtil::getHeldItemId() == 130) PlayerUtil::tryPlaceBlock(pos, false, packetPlace);
				}
			}
			delay = 0;
		}
		else delay++;
	}
	InventoryUtil::switchSlot(oldSlot);
	if (mobEq && PlayerUtil::selectedSlotServerSide != oldSlot) InventoryUtil::sendMobEquipment(oldSlot);
}