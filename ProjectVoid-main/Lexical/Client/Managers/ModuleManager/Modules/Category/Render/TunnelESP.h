#pragma once
#include "../../ModuleBase/Module.h"

class TunnelESP : public Module {
public:
	std::unordered_set<BlockPos> tunnelList;
	UIColor color = UIColor(255, 255, 255, 70);
public:
	bool isTunnel(const BlockPos& blockPos);
public:
	TunnelESP();

	void onLevelRender() override;
};