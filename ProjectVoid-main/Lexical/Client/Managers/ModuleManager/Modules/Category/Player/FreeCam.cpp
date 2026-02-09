#include "FreeCam.h"

FreeCam::FreeCam() : Module("Freecam", "FreeCam Fly", Category::PLAYER) {
    registerSetting(new SliderSetting<float>("Speed", "speed", &Speed, 1.f, 0.2f, 5.f));
}

void FreeCam::onEnable() {
    isCancelMovePacket = false;
    spoofedPos = Game::getLocalPlayer()->getPos();
    Game::getLocalPlayer()->setPos(spoofedPos);
}

void FreeCam::onDisable() {
    isCancelMovePacket = false;
    Game::getLocalPlayer()->setPos(spoofedPos);
}

void FreeCam::onNormalTick(LocalPlayer* localPlayer) {
    localPlayer->stateVector->velocity = (0, 0, 0);
    if (Game::canUseMoveKeys()) {
        float yaw = localPlayer->rotation->presentRot.y;
        bool isForward = Game::isKeyDown('W');
        bool isLeft = Game::isKeyDown('A');
        bool isBackward = Game::isKeyDown('S');
        bool isRight = Game::isKeyDown('D');
        bool isUp = Game::isKeyDown(VK_SPACE);
        bool isDown = Game::isKeyDown(VK_SHIFT);
        Vec2<int> moveValue;
        if (isRight) moveValue.x += 1;
        if (isLeft) moveValue.x -= 1;
        if (isForward) moveValue.y += 1;
        if (isBackward) moveValue.y -= 1;
        float verticalSpeed = 0;
        if (isUp) verticalSpeed += Speed;
        if (isDown) verticalSpeed -= Speed;
        float angleRad = std::atan2(moveValue.x, moveValue.y);
        float angleDeg = angleRad * (180.f / PI);
        yaw += angleDeg;
        Vec3<float> moveVec = Vec3<float>(0.f, verticalSpeed, 0.f);
        if ((moveValue.x != 0) || (moveValue.y != 0)) {
            float calcYaw = (yaw + 90.f) * (PI / 180.f);
            moveVec.x = cos(calcYaw) * Speed;
            moveVec.z = sin(calcYaw) * Speed;
        }
        Vec3<float> newPos(
            localPlayer->stateVector->pos.x + moveVec.x,
            localPlayer->stateVector->pos.y + moveVec.y,
            localPlayer->stateVector->pos.z + moveVec.z
        );
        localPlayer->setPos(newPos);
    }
}

void FreeCam::onSendPacket(Packet* packet, bool& cancel) {
    if (packet->getId() == PacketID::MovePlayer) {
        auto* inputPkt = static_cast<MovePlayerPacket*>(packet);
        if (!inputPkt) return;
        inputPkt->mPosition = spoofedPos;
        isCancelMovePacket = true;
    }
    if (packet->getId() == PacketID::PlayerAuthInput) {
        auto* inputPkt = static_cast<PlayerAuthInputPacket*>(packet);
        if (!inputPkt) return;
        inputPkt->position = spoofedPos;
    }
    else {
        isCancelMovePacket = false;
    }
}