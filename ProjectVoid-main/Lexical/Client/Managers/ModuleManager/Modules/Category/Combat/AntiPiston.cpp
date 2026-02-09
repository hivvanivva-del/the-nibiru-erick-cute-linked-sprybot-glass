#include "AntiPiston.h"
#include "../../../ModuleManager.h"

AntiPiston::AntiPiston() : Module("AntiPiston", "Mines pistons automatically", Category::COMBAT) {
	registerSetting(new BoolSetting("Mine Redstone", "NULL", &mineRedstone, false));
	registerSetting(new BoolSetting("Mine Piston", "NULL", &minePiston, false));
	registerSetting(new BoolSetting("Smart", "Only mine when theres a target nearby", &smart, false));
}

void AntiPiston::onNormalTick(LocalPlayer* localPlayer) {
	static std::vector<BlockPos> mineList;
	static std::vector<Actor*> actorList;
	static bool b = false;
	GameMode* gm = localPlayer->gameMode;
	actorList.clear();
	if (smart) {
		for (Actor* actor : localPlayer->level->getRuntimeActorList()) {
			if (!TargetUtil::isTargetValid(actor, false, true, 8.f)) continue;
			actorList.push_back(actor);
		}
		if (actorList.empty()) return;
	}
	static PacketMine* packetMine = ModuleManager::getModule<PacketMine>();
	if (!packetMine->isEnabled()) packetMine->setEnabled(true);

	Vec3<float> plrPos = localPlayer->getPos();
	mineList.clear();
	for (int x = -3; x <= 3; x++) {
		for (int y = -2; y <= 2; y++) {
			for (int z = -3; z <= 3; z++) {
				BlockPos blockPos = plrPos.add(x, y, z).CastTo<int>();
				int blockId = WorldUtil::getBlock(blockPos)->blockLegacy->blockId;
				if ((blockId == 29 || blockId == 33) && minePiston) mineList.push_back(blockPos);
				if ((blockId == 130 || blockId == 76) && mineRedstone) mineList.push_back(blockPos);
			}
		}
	}
	if (mineList.empty()) return;
	std::sort(mineList.begin(), mineList.end(), [plrPos](BlockPos b1, BlockPos b2) {
		return WorldUtil::distanceToBlock(plrPos, b1) < WorldUtil::distanceToBlock(plrPos, b2);
		});
	gm->startDestroyBlock(mineList.front(), 0, b);
	packetMine->mineBlock(mineList.front(), 0);
	int blockId = WorldUtil::getBlock(mineList.front())->blockLegacy->blockId;
	if (blockId == 33 || blockId == 76) gm->destroyBlock(mineList.front(), 0); // little birdy told me u can instamine pistons
}