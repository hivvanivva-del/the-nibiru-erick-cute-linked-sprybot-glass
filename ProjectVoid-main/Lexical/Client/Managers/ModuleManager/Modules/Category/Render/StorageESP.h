#pragma once
#include "../../ModuleBase/Module.h"

class StorageESP : public Module {
public:
	std::unordered_map<BlockPos, UIColor> blockList;
	StorageESP();
public:
	bool trace = true;
	int radius = 64;
	int alpha = 50;
	int tracerAlpha = 255;
public:
	UIColor getColor(const BlockPos& blockPos);
	void onLevelRender() override;
};