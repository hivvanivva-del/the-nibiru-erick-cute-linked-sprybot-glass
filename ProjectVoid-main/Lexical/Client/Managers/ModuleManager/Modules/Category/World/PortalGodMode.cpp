#include "PortalGodMode.h"

PlayerAuthInputPacket* storedAuthPacket = nullptr;

PortalGodMode::PortalGodMode()
    : Module("PortalGodMode", "Makes you invincible after going through a portal", Category::WORLD) {
}
void PortalGodMode::onNormalTick(LocalPlayer* localPlayer) {
    if (!Game::getLocalPlayer()) return;
}

void PortalGodMode::onSendPacket(Packet* packet, bool& cancel) {
    if (!packet) return;

    if (packet->getId() == PacketID::PlayerAuthInput) {
        if (packet->getId() == PacketID::ChangeDimension) {
            PlayerAuthInputPacket* authPacket = (PlayerAuthInputPacket*)packet;
            auto player = Game::getLocalPlayer();
            if (!player) return;
            authPacket->rotation.y = player->rotation->presentRot.y * 2147483648;
            authPacket->rotation.x = player->rotation->presentRot.x * 2147483648;
        }
    }
}
