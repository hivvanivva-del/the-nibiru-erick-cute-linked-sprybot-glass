#include "ClientSideInv.h"

ClientSideInv::ClientSideInv() : Module("ClientSideInv", "Sets your inventory as client sided.", Category::WORLD) {}

void ClientSideInv::onSendPacket(Packet* packet, bool& cancel) {
	if (!packet) return;
	if (packet->getId() == PacketID::PlayerAuthInput) {
		PlayerAuthInputPacket* authPacket = (PlayerAuthInputPacket*)packet;
	}
	if (packet->getId() == PacketID::InventoryTransaction) {
		InventoryTransactionPacket* invPkt = (InventoryTransactionPacket*)packet;
		invPkt->isClientSide = true;
	}
}