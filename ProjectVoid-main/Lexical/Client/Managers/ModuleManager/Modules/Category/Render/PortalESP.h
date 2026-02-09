#pragma once
#include "../../ModuleBase/Module.h"

class PortalESP : public Module {
public:
	PortalESP();
	std::unordered_set<BlockPos> portalList;
public:
	bool trace = true;
	int radius = 64;

	UIColor color = UIColor(255, 0, 155, 50);
	UIColor lineColor = UIColor(255, 0, 155, 70);

	UIColor tracerColor = UIColor(255, 0, 155, 255);
public:
	void onLevelRender() override;
};