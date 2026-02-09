#include "PacketHand.h"
#include "../../../ModuleManager.h"

void PacketHand::onBuild(const BlockPos& blockPos, const uint8_t face, bool& cancel) {
	if (eatMultiTask) eatingTicks = -5;
	if (!packetPlace) return;
	LocalPlayer* localPlayer = Game::getLocalPlayer();
	cancel = true;
	PlayerInventory* plrInv = localPlayer->playerInventory;
	std::unique_ptr<ItemUseInventoryTransaction> itemUseInvTransac = ItemUseInventoryTransaction::make_unique();
	itemUseInvTransac->actionType = ItemUseInventoryTransaction::ActionType::Place;
	itemUseInvTransac->blockPos = blockPos;
	itemUseInvTransac->targetBlockRuntimeId = *Game::clientInstance->getRegion()->getBlock(blockPos)->getRuntimeId();
	itemUseInvTransac->face = face;
	itemUseInvTransac->slot = plrInv->selectedSlot;
	itemUseInvTransac->itemInHand = NetworkItemStackDescriptor(plrInv->container->getItem(plrInv->selectedSlot));
	itemUseInvTransac->playerPos = localPlayer->getPos();
	itemUseInvTransac->clickPos = Vec3<float>(0.5f, 0.5f, 0.5f);
	InventoryTransactionPacket pk(std::move(itemUseInvTransac));
	localPlayer->level->getPacketSender()->send(&pk);
}

void PacketHand::onAttack(Actor* actor, bool& cancel) {
	if (!eatMultiTask) eatingTicks = -20;
	if (!packetAttack) return;
	LocalPlayer* localPlayer = Game::getLocalPlayer();
	cancel = true;
	std::unique_ptr<ItemUseOnActorInventoryTransaction> attackInvTransac = ItemUseOnActorInventoryTransaction::make_unique();
	attackInvTransac->actorRuntimeId = actor->getRuntimeIDComponent()->runtimeId.id;
	attackInvTransac->actionType = ItemUseOnActorInventoryTransaction::ActionType::Attack;
	attackInvTransac->slot = localPlayer->playerInventory->selectedSlot;
	attackInvTransac->itemInHand = NetworkItemStackDescriptor(localPlayer->playerInventory->container->getItem(attackInvTransac->slot));
	attackInvTransac->playerPos = localPlayer->getPos();
	attackInvTransac->clickPos = Vec3<float>(0.f, 0.f, 0.f);
	InventoryTransactionPacket itp(std::move(attackInvTransac));
	localPlayer->level->getPacketSender()->send(&itp);
}

void PacketHand::onNormalTick(LocalPlayer* localPlayer) {
	if (!packetEat) return;
	GameMode* gm = localPlayer->gameMode;
	uint8_t oldSlot = InventoryUtil::getSelectedSlot();
	uint8_t gapple = InventoryUtil::findItemSlotInHotbar(263);
	if (eatingTicks == 0) {
		InventoryUtil::switchSlot(gapple);
		gm->baseUseItem(InventoryUtil::getItem(gapple));
		InventoryUtil::switchSlot(oldSlot);
		eatingTicks++;
	}
	else if (eatingTicks > eatCooldown) {
		if (Game::isKeyDown(eatKey) || eatKey == 0x0) {
			InventoryUtil::switchSlot(gapple);
			gm->baseUseItem(InventoryUtil::getItem(gapple));
			gm->releaseUsingItem();
			eatingTicks = -5;
			InventoryUtil::switchSlot(oldSlot);
		}
	}
	else eatingTicks++;
}

void PacketHand::onD2DRender() {
	if (!packetEat) return;
	LocalPlayer* localPlayer = Game::getLocalPlayer();
	if (localPlayer == nullptr) return;
	const Vec2<float>& windowSize = Game::clientInstance->guiData->windowSizeReal;
	static Colors* colorMod = ModuleManager::getModule<Colors>();
	int eatTicks = eatingTicks;
	if (eatingTicks < 0) eatTicks = 0;
	const float fraction = eatTicks / (float)eatCooldown;
	const uint8_t percent = fraction * 100;
	const Vec2<float>& textPos = Vec2<float>((windowSize.x / 2) - (D2D::getTextWidth("Eating... 100%", 1.f) / 2.f), windowSize.y - 200.f);
	const std::string& text = "Eating... " + std::to_string(percent) + "%";
	const Vec4<float>& rect = Vec4<float>(
		textPos.x + D2D::getTextWidth("Eating... 100%", 1.f) + 3.f, textPos.y,
		textPos.x - 3.f, textPos.y + D2D::getTextHeight(text, 1.f)
	);
	float progress = percent / 100.f;
	if (progress < 0.f) progress = 0.f;
	const Vec4<float>& colorRect = Vec4<float>(
		rect.x + (rect.z - rect.x) * fraction, rect.y, rect.z, rect.w
	);
	const UIColor& mainColor = colorMod->getColor();
	D2D::fillRectangle(rect, UIColor(0, 0, 0, 100));
	D2D::fillRectangle(colorRect, mainColor);
	D2D::drawText(textPos, text, UIColor(255, 255, 255));
}