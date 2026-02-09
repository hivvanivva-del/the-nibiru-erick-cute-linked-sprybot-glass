#pragma once
#include "../../ModuleBase/Module.h"

class AutoWither : public Module {
private:
	BlockPos blockPos;
	std::vector<BlockPos> soulBlocks;
	std::vector<BlockPos> headBlocks;
	int skullMode = 0;
public:
	AutoWither();
	void onBuild(const BlockPos& buildPos, const uint8_t face, bool& cancel) override;
	void onNormalTick(LocalPlayer* localPlayer) override;
};