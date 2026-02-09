#pragma once
#include "../../ModuleBase/Module.h"

class AutoTunnel : public Module {
private:
	int mode = 0;
	std::vector<BlockPos> blockList;
public:
	AutoTunnel();

	BlockPos getOffset(LocalPlayer* localPlayer);
	void onNormalTick(LocalPlayer* localPlayer) override;
};