#include "Criticals.h"
Criticals::Criticals() : Module("Criticals", "Each hit is a critical hit", Category::COMBAT) {
	registerSetting(new EnumSetting("Mode", "NULL", { "Nukkit", "Geyser", "Geyser Strict", "Jump"}, &mode, 0));
}

std::string Criticals::getModeText() {
	switch (mode) {
	case 0: {
		return "Nukkit";
		break;
	}
	case 1: {
		return "Geyser";
		break;
	}
	case 2: {
		return "Geyser Strict";
		break;
	}
	case 3: {
		return "Glide";
		break;
	}
	}
	return "NULL";
}

void Criticals::onSendPacket(Packet* packet, bool& cancel) {
	static float offsetY = 0.0f;
	static bool wasOnGround = false;
	LocalPlayer* localPlayer = Game::getLocalPlayer();
	if (localPlayer == nullptr) {
		offsetY = 0.0f;
		return;
	}

	if (mode == 0) {
		if (packet->getId() == PacketID::PlayerAuthInput) {
			PlayerAuthInputPacket* paip = static_cast<PlayerAuthInputPacket*>(packet);
			paip->position.y -= offsetY;
			offsetY += 0.012f;
			if (offsetY >= 0.24f) {
				offsetY = 0.0f;
			}
		}
	}
	else if (mode == 3) {
		float& yVelocity = localPlayer->stateVector->velocity.y;
		if (!MoveUtil::isMoving()) {
			if (localPlayer->onGround()) localPlayer->jumpFromGround();
			else yVelocity = -0.0625;
		}
		else {
			if (localPlayer->onGround()) wasOnGround = true;
			if (wasOnGround && !localPlayer->onGround()) wasOnGround = false;
			if (!Game::isKeyDown(VK_SPACE) && wasOnGround) localPlayer->stateVector->velocity.y = -1.f;
		}
	}
}

void Criticals::sendMovePacket(double height) {
	const Vec3<float>& playerPos = Game::getLocalPlayer()->getPos().sub(0, 1.6f, 0).floor();
	std::shared_ptr<Packet> packet = MinecraftPackets::createPacket(PacketID::MovePlayer);
	MovePlayerPacket* movePkt = (MovePlayerPacket*)packet.get();
	movePkt->mPosition = playerPos.add(static_cast<float>(height));
	movePkt->headYaw = Game::getLocalPlayer()->getActorHeadRotationComponent()->headYaw;
	Game::getLocalPlayer()->level->getPacketSender()->send(movePkt);
}

void Criticals::onAttack(Actor* actor, bool& cancel) {
	// meteor client <3
	if (mode == 1) {
		sendMovePacket(0.0625);
		sendMovePacket(0.f);
	}
	else if (mode == 2) {
		sendMovePacket(0.11f);
		sendMovePacket(0.1100013579);
		sendMovePacket(0.1100013579);
	}
}