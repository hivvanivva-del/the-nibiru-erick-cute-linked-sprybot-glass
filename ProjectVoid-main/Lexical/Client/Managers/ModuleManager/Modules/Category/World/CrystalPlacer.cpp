#include "CrystalPlacer.h"

CrystalPlacer::CrystalPlacer() : Module("CrystalPlacer", "Whenever you place obby it places end crystals automatically", Category::MISC) {
}

void CrystalPlacer::onNormalTick(LocalPlayer* localPlayer) {
	static bool place = false;
	const BlockPos& pointing = localPlayer->level->getHitResult()->blockPos;
	uint8_t oldSlot = InventoryUtil::getSelectedSlot();
	if (pointing == BlockPos(0, 0, 0) || WorldUtil::getBlock(pointing)->blockLegacy->blockId == 0) return;
	if (InventoryUtil::getHeldItemId() == 49 && Game::isRightClickDown()) {
		oldSlot = InventoryUtil::getSelectedSlot();
		if (WorldUtil::getBlock(pointing)->blockLegacy->blockId == 49) place = true;
	}
	if (place) {
		InventoryUtil::switchSlot(InventoryUtil::findItemSlotInHotbar(758));
		InventoryUtil::sendMobEquipment(InventoryUtil::findItemSlotInHotbar(758));
		localPlayer->gameMode->buildBlock(pointing, 0, false);
		if (InventoryUtil::getSelectedSlot() != oldSlot) InventoryUtil::switchSlot(oldSlot);
		if (PlayerUtil::selectedSlotServerSide != oldSlot) InventoryUtil::sendMobEquipment(oldSlot);
		place = false;
	}
}