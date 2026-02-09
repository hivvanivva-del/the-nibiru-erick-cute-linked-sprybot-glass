#include "AutoTunnel.h"
#include "../../../ModuleManager.h"

AutoTunnel::AutoTunnel() : Module("Tunnel", "Mines the 2 blocks in front of u or the block ur looking at, depends on PacketMine", Category::MISC) {
	registerSetting(new EnumSetting("Mode", "NULL", { "Normal", "Looking" }, &mode, 0));
}

BlockPos AutoTunnel::getOffset(LocalPlayer* localPlayer) {
	float rotation = localPlayer->rotation->presentRot.y;
	if (rotation < 0) rotation += 360.0;
	if ((0 <= rotation && rotation < 45) || (315 <= rotation && rotation < 360)) return BlockPos(0, 0, 1);
	else if (45 <= rotation && rotation < 135) return BlockPos(-1, 0, 0);
	else if (135 <= rotation && rotation < 225) return BlockPos(0, 0, -1);
	else if (225 <= rotation && rotation < 315) return BlockPos(1, 0, 0);
	return BlockPos(1, 0, 0);
}

void AutoTunnel::onNormalTick(LocalPlayer* localPlayer) {
	static bool b = false;
	static PacketMine* packetMine = ModuleManager::getModule<PacketMine>();
	GameMode* gm = localPlayer->gameMode;
	if (!packetMine->isEnabled()) {
		packetMine->setEnabled(true);
	}

	blockList.clear();
	BlockPos offset = getOffset(localPlayer);
	BlockPos playerPos = localPlayer->getPos().floor().sub(0, 1, 0).CastTo<int>();

	if (mode == 0) {
		for (int i = 1; i < 4; i++) {
			blockList.push_back(playerPos.add(offset.x * i, 0, offset.z * i));
			blockList.push_back(playerPos.add(offset.x * i, 1, offset.z * i));
		}
	}
	else {
		BlockPos looking = localPlayer->level->getHitResult()->blockPos;
		if (looking != BlockPos{}) {
			blockList.push_back(looking);
		}
	}

	if (blockList.empty()) {
		return;
	}

	for (int i = 0; i < blockList.size(); i++) {
		if (!WorldUtil::getBlock(blockList[i])->blockLegacy->isSolid()) {
			blockList.erase(blockList.begin() + i);
			break;
		}
	}

	std::sort(blockList.begin(), blockList.end(), [&](const BlockPos& b1, const BlockPos& b2) {
		return b1.dist(playerPos) < b2.dist(playerPos);
	});
	
	if ((localPlayer->gameMode->destroyProgress == 0.f || packetMine->getBreakPos() == BlockPos(0, 0, 0)) && WorldUtil::getBlock(blockList[0])->blockLegacy->isSolid()) {
		packetMine->mineBlock(blockList[0], 0);
		gm->startDestroyBlock(blockList[0], 0, b);
	}
}