#include "Trident.h"

Trident::Trident() : Module("Trident", "Trident exploit.", Category::WORLD) {}

void Trident::onNormalTick(LocalPlayer* localPlayer) {
	if (!Game::getLocalPlayer()) return;
	Game::getLocalPlayer()->setStatusFlag(ActorFlags::Damagenearbymobs, true);
}
void Trident::onSendPacket(Packet* packet, bool& cancel) {
	if (!packet) return;
	if (packet->getId() == PacketID::PlayerAuthInput) {
		PlayerAuthInputPacket* authPacket = (PlayerAuthInputPacket*)packet;
		Game::getLocalPlayer()->setStatusFlag(ActorFlags::Damagenearbymobs, true);
		authPacket->mInputData |= InputData::AutoJumpingInWater;
	}
	if (packet->getId() == PacketID::InventoryTransaction) {
		InventoryTransactionPacket* invPkt = (InventoryTransactionPacket*)packet;
		invPkt->isClientSide = true;
	}
}