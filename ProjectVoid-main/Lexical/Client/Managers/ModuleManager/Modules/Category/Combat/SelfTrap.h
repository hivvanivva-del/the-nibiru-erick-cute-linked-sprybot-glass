#pragma once
#include "../../ModuleBase/Module.h"

class SelfTrap : public Module {
private:
	int mode = 0;
	int switchMode = 0;
	int bpt = 1;
	int headSpace = 0;
	bool airplace = false;
	bool packetPlace = false;
	bool button = false;
private:
	inline AABB getBlockAABB(const BlockPos& blockPos) {
		AABB result;
		result.lower = blockPos.CastTo<float>();
		result.upper = result.lower.add(Vec3<float>(1.f, 1.f, 1.f));
		return result;
	}
public:
	SelfTrap();
	virtual void onNormalTick(LocalPlayer* localPlayer) override;
};