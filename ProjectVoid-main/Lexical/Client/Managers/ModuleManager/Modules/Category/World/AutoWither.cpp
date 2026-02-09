#include "AutoWither.h"

AutoWither::AutoWither() : Module("AutoWither", "Place a soul sand and itll spawn a wither for u!", Category::MISC) {
	registerSetting(new EnumSetting("Skull", "How we place the skull", { "All", "No Center", "None" }, &skullMode, 0));
}

void AutoWither::onBuild(const BlockPos& buildPos, const uint8_t face, bool& cancel) {
	if (InventoryUtil::getHeldItemId() == 88) blockPos = buildPos;
}

void AutoWither::onNormalTick(LocalPlayer* localPlayer) {
	static int fdelay = 0;

	soulBlocks.clear();
	headBlocks.clear();

	if (blockPos == BlockPos{}) {
		fdelay = 0;
		return;
	}

	float rotation = Game::getLocalPlayer()->rotation->presentRot.y;
	if (rotation < 0) {
		rotation += 360.0;
	}
	if (((0 <= rotation && rotation < 45) || (315 <= rotation && rotation < 360)) || (135 <= rotation && rotation < 225)) {
		soulBlocks.push_back(blockPos.add(1, 2, 0));
		soulBlocks.push_back(blockPos.add(-1, 2, 0));

		headBlocks.push_back(blockPos.add(1, 3, 0));
		headBlocks.push_back(blockPos.add(-1, 3, 0));
	}
	else if ((45 <= rotation && rotation < 135) || (225 <= rotation && rotation < 315)) {
		soulBlocks.push_back(blockPos.add(0, 2, 1));
		soulBlocks.push_back(blockPos.add(0, 2, -1));

		headBlocks.push_back(blockPos.add(0, 3, 1));
		headBlocks.push_back(blockPos.add(0, 3, -1));
	}

	if (skullMode != 1) {
		headBlocks.push_back(blockPos.add(0, 3, 0));
	}

	if (soulBlocks.empty() || headBlocks.empty()) {
		return;
	}

	int oldSlot = InventoryUtil::getSelectedSlot();

	PlayerUtil::tryPlaceBlock(blockPos.add(0, 1, 0), false, true);

	fdelay++;
	if (fdelay >= 1) {
		PlayerUtil::tryPlaceBlock(blockPos.add(0, 2, 0), false, true);
		if (fdelay >= 2) {
			for (BlockPos pos : soulBlocks) {
				PlayerUtil::tryPlaceBlock(pos, false, true);
			}
			if (fdelay >= 4) {
				InventoryUtil::switchSlot(InventoryUtil::findItemSlotInHotbar(532));
				if (InventoryUtil::getHeldItemId() == 532) {
					for (BlockPos pos : headBlocks) {
						PlayerUtil::tryPlaceBlock(pos, false, true);
					}
				}
				InventoryUtil::switchSlot(oldSlot);
				blockPos = BlockPos{};
				soulBlocks.clear();
				headBlocks.clear();
				fdelay = 0;
			}
		}
	}
}