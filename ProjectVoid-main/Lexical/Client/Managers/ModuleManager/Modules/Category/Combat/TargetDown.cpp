#include "TargetDown.h"
#include "../../../ModuleManager.h"

void TargetDown::onNormalTick(LocalPlayer* localPlayer) {
	targetList.clear();
	static PacketMine* packetMine = ModuleManager::getModule<PacketMine>();
	static bool b = false;
	if (!packetMine->isEnabled()) {
		packetMine->setEnabled(true);
		return;
	}
	for (Actor* actor : localPlayer->level->getRuntimeActorList()) {
		if (!TargetUtil::isTargetValid(actor, false, true, targetRange)) continue;
		targetList.push_back(actor);
	}
	if (targetList.empty()) return;
	std::sort(targetList.begin(), targetList.end(), TargetUtil::sortByDist);
	Actor& mainActor = *targetList[0];
	const Vec3<float>& actorPos = mainActor.getPos().floor().sub(0.f, 1.f, 0.f);
	const BlockPos& minePos = actorPos.CastTo<int>().sub(0, 1, 0);
	if ((!priority && (localPlayer->gameMode->destroyProgress == 0.f || packetMine->getBreakPos() == BlockPos(0, 0, 0))) || (priority && packetMine->getBreakPos() != minePos)) {
		packetMine->mineBlock(minePos, 0);
		localPlayer->gameMode->startDestroyBlock(minePos, 0, b);
	}
}