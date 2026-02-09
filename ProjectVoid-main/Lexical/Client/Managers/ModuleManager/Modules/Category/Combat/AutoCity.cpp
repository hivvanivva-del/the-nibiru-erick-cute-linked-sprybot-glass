#include "AutoCity.h"
#include "../../../ModuleManager.h"

AutoCity::AutoCity() : Module("AutoCity", "Automatically mines their surround", Category::COMBAT) {
	registerSetting(new SliderSetting<float>("Range", "NULL", &range, 6.f, 1.f, 12.f));
	//registerSetting(new BoolSetting("Face", "Mine their face surround", &face, false));
	registerSetting(new BoolSetting("Burrow", "Mine their burrow", &burrow, false));
	registerSetting(new BoolSetting("Priority", "priority mining surr before packetmine", &priority, false));
}

BlockPos AutoCity::getMinePos(Actor* actor) const {
	static std::vector<BlockPos> mineList;
	mineList.clear();
	static BlockPos checkList[4] = { {1, 0, 0}, {-1, 0, 0}, {0, 0, 1}, {0, 0, -1} };
	const Vec3<float>& actorPos = actor->getPos().floor().sub(0, 1, 0);
	if (burrow && !WorldUtil::canBuildOn(actorPos.CastTo<int>())) return actorPos.CastTo<int>();
	for (const BlockPos& check : checkList) {
		const BlockPos& pos = actorPos.CastTo<int>().add(check);
		Block* block = WorldUtil::getBlock(pos);
		if (block->blockLegacy->blockId != 7 && !WorldUtil::canBuildOn(pos)) mineList.push_back(pos);
	}
	std::sort(mineList.begin(), mineList.end(), [&](BlockPos b1, BlockPos b2) {
		return WorldUtil::distanceToBlock(actorPos, b1) < WorldUtil::distanceToBlock(actorPos, b2);
		});
	if (mineList.empty()) return BlockPos(0, 0, 0);
	return mineList[0];
}

void AutoCity::onNormalTick(LocalPlayer* localPlayer) {
	static bool b = false;
	static PacketMine* packetMine = ModuleManager::getModule<PacketMine>();
	GameMode* gm = localPlayer->gameMode;
	actorList.clear();
	if (localPlayer->getItemUseDuration() > 0) return;
	for (Actor* actor : localPlayer->level->getRuntimeActorList()) {
		if (!TargetUtil::isTargetValid(actor, false, true, range)) continue;
		actorList.push_back(actor);
	}
	std::sort(actorList.begin(), actorList.end(), TargetUtil::sortByDist);
	if (actorList.empty()) return;
	const BlockPos& minePos = getMinePos(actorList[0]);
	if (!packetMine->isEnabled()) packetMine->setEnabled(true);
	if ((!priority && (localPlayer->gameMode->destroyProgress == 0.f || packetMine->getBreakPos() == BlockPos(0, 0, 0))) || (priority && packetMine->getBreakPos() != minePos)) {
		packetMine->mineBlock(minePos, 0);
		gm->startDestroyBlock(minePos, 0, b);
	}
}