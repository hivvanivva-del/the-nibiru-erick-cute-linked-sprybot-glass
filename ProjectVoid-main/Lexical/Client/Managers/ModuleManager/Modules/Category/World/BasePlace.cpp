#include "BasePlace.h"

BasePlace::BasePlace() : Module("BasePlace", "Places obsidian on the target to crystal them!", Category::COMBAT) {
	registerSetting(new SliderSetting<float>("Target Dist", "Target range", &targetRange, 12, 1, 12));
	registerSetting(new BoolSetting("Support", "Like clutch mode in scaffold, adds support beams for flatten so we can place", &support, false));
	registerSetting(new SliderSetting<int>("Proximity", "Locates the nearest block around the target so we can add support beams, the more proximity the more radius", &proximity, 5, 1, 6));
	registerSetting(new SliderSetting<int>("Radius", "How big the flatten will be", &flattenRadius, 1, 1, 5));
	registerSetting(new SliderSetting<int>("Delay", "Place delay", &placeDelay, 0, 0, 20));
    registerSetting(new BoolSetting("MobEq", "Sends mob eq packets", &mobEq, false));
	registerSetting(new BoolSetting("Self", "Test on sef", &self, false));
}

bool BasePlace::buildBlock(const BlockPos& pos, Actor* actor) {
    LocalPlayer* localPlayer = Game::getLocalPlayer();
    if (targetList.empty()) return false;
    if (support) {
        std::vector<BlockPos> checkList;
        BlockPos nearestBlock;
        if (checkList.empty()) {
            for (int x = -proximity; x <= proximity; x++) {
                for (int y = -proximity; y <= proximity; y++) {
                    for (int z = -proximity; z <= proximity; z++) {
                        checkList.emplace_back(pos.x + x, pos.y + y, pos.z + z);
                    }
                }
            }
        }
        const Vec3<float>& actorPos = actor->getPos().floor().sub(0, 1, 0);
        const BlockPos& belowPos = actorPos.CastTo<int>().sub(0, 2, 0);

        std::sort(checkList.begin(), checkList.end(), [&](const BlockPos& start, const BlockPos& end) {
            return belowPos.dist(start) < belowPos.dist(end);
            });

        std::vector<BlockPos>::iterator it = std::find_if(checkList.begin(), checkList.end(), [](const BlockPos& current) {
            return !WorldUtil::canBuildOn(current);
        });

        if (it != checkList.end()) nearestBlock = *it;
        else return false;

        std::vector<BlockPos> offset;

        for (int x = -1; x <= 1; x++) {
            for (int y = -1; y <= 1; y++) {
                for (int z = -1; z <= 1; z++) {
                    offset.emplace_back(nearestBlock.x + x, nearestBlock.y + y, nearestBlock.z + z);
                }
            }
        }

        std::sort(offset.begin(), offset.end(), [&](const BlockPos& start, const BlockPos& end) {
            return belowPos.dist(start) < belowPos.dist(end);
        });
        static float iScan = 0;
        if (TimerUtil::hasReached<ticks>(&iScan, placeDelay)) {
            for (const BlockPos& off : offset) {
                if (PlayerUtil::canPlaceBlock(off)) {
                    if (PlayerUtil::tryPlaceBlock(off)) {
                        localPlayer->swing();
                        break;
                    }
                }
            }
        }
    } else PlayerUtil::tryPlaceBlock(pos);
    return true;
}

void BasePlace::onNormalTick(LocalPlayer* localPlayer) {
	targetList.clear();
	for (Actor* actor : localPlayer->level->getRuntimeActorList()) {
		if (!TargetUtil::isTargetValid(actor)) continue;
		if (actor->getEyePos().dist(localPlayer->getEyePos()) > targetRange) continue;
		targetList.push_back(actor);
	}
	if (self) targetList.push_back(localPlayer);
	if (targetList.empty()) return;
	for (Actor* target : targetList) {
		Vec3<float> actorPos = target->getPos().floor().sub(0, 1, 0);
		int oldSlot = InventoryUtil::getSelectedSlot();
		InventoryUtil::switchSlot(InventoryUtil::findItemSlotInHotbar(49));
        if (mobEq) InventoryUtil::sendMobEquipment(InventoryUtil::findItemSlotInHotbar(49));
		if (support) BasePlace::buildBlock(actorPos.CastTo<int>().sub(0, 2, 0), target);
		for (int x = -flattenRadius; x <= flattenRadius; x++) {
			for (int z = -flattenRadius; z <= flattenRadius; z++) {
				BlockPos pos(actorPos.x + x, actorPos.y - 1, actorPos.z + z);
				if (!WorldUtil::canBuildOn(actorPos.sub(0.f, 1.f, 0.f).CastTo<int>())) PlayerUtil::tryPlaceBlock(pos);
				else PlayerUtil::tryPlaceBlock(pos.sub(0, 1, 0));
			}
		}
		InventoryUtil::switchSlot(oldSlot);
        if (mobEq && PlayerUtil::selectedSlotServerSide != oldSlot) InventoryUtil::sendMobEquipment(oldSlot);
		break;
	}
}