#pragma once
#include "../../ModuleBase/Module.h"

class AutoTrap : public Module {
private:
	std::vector<BlockPos> placeList;
	int bpt = 1;
	int maxTarget = 1;
	int range = 6;
	bool packetPlace = false;
	bool airplace = false;
	int switchMode = 0;
	int headSpace = 0;
	UIColor color = UIColor(255, 255, 255, 40);
	int trapMode = 0;
private:
	int oldSlot = -1;
	std::unordered_map<BlockPos, float> fadeList;
	inline AABB getBlockAABB(const BlockPos& blockPos) {
		AABB result;
		result.lower = blockPos.CastTo<float>();
		result.upper = result.lower.add(Vec3<float>(1.f, 1.f, 1.f));
		return result;
	}
public:
	AutoTrap();
	virtual void onNormalTick(LocalPlayer* player) override;
	virtual void onLevelRender() override;
};