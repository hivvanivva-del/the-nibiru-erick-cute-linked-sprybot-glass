#pragma once
#include "../../ModuleBase/Module.h"

class MaceKiller : public Module {
public:
	int height = 10;
public:
	int getMaxHeightAbovePlayer() {
		LocalPlayer* lp = Game::getLocalPlayer();
		Vec3<float> playerPos = lp->getPos().floor().sub(0, 1, 0);
		int maxHeight = playerPos.y + height;
		for (int i = maxHeight; i > playerPos.y; i--) {
			BlockPos isAir = BlockPos(playerPos.x, i, playerPos.y);
			BlockPos isAir1 = isAir.add(0, 1, 0);
			if (WorldUtil::canBuildOn(isAir) && WorldUtil::canBuildOn(isAir1)) return i - playerPos.y;
		}
	}
public:
	MaceKiller() : Module("MaceKiller", "YESSS", Category::COMBAT) {
		registerSetting(new SliderSetting<int>("Height", "NULL", &height, height, 1, 128));
	}
	virtual void onAttack(Actor* actor, bool& cancel) override {
		LocalPlayer* lp = Game::getLocalPlayer();
		if (InventoryUtil::getHeldItemId() == 326) {
			int blocks = getMaxHeightAbovePlayer();
			int packetsNeeded = (int)ceil(abs(blocks / 20));
			if (packetsNeeded > 20) packetsNeeded = 1;
			Vec3<float> lastPos = lp->getPos().floor().sub(0, 1, 0);
			BlockPos isAir = lastPos.add(0, blocks, 0).CastTo<int>();
			BlockPos isAir1 = lastPos.add(0, blocks + 1, 0).CastTo<int>();
			if (WorldUtil::canBuildOn(isAir) && WorldUtil::canBuildOn(isAir1)) {
				for (int i = 0; i < packetsNeeded - 1; i++) {
					std::shared_ptr<Packet> sharedPkt = MinecraftPackets::createPacket(PacketID::MovePlayer);
					MovePlayerPacket* movePkt = (MovePlayerPacket*)sharedPkt.get();
					movePkt->onGround = false;
					lp->level->getPacketSender()->send(movePkt);
				}
				{
					std::shared_ptr<Packet> sharedPkt = MinecraftPackets::createPacket(PacketID::MovePlayer);
					MovePlayerPacket* movePkt = (MovePlayerPacket*)sharedPkt.get();
					movePkt->onGround = false;
					movePkt->mPosition = lastPos.add(blocks);
					lp->level->getPacketSender()->send(movePkt);
				}
				{
					std::shared_ptr<Packet> sharedPkt = MinecraftPackets::createPacket(PacketID::MovePlayer);
					MovePlayerPacket* movePkt = (MovePlayerPacket*)sharedPkt.get();
					movePkt->mPosition = lastPos;
					movePkt->onGround = false;
					lp->level->getPacketSender()->send(movePkt);
				}
			}
		}
	}
};