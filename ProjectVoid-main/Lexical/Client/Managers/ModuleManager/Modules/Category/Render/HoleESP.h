#pragma once
#include "../../ModuleBase/Module.h"

class HoleESP : public Module {
private:
	int hRadius = 8;
	int vRadius = 4;
	bool ignoreOwn = false;
	bool fill = true;
	bool outline = false;
	UIColor bHoleColor = UIColor(0, 255, 0, 50);
	UIColor oHoleColor = UIColor(255, 0, 0, 50);
	UIColor mHoleColor = UIColor(255, 255, 0, 50);
public:
	enum class HoleType {
		Invalid,
		Bedrock,
		Obsidian,
		Mixed
	};
private:
	std::unordered_map<BlockPos, std::pair<HoleType, float>> holeMap;
	size_t holeCount = 0;

	void renderHole(const BlockPos& holePos, HoleType holeType, float alpha);
	void renderHoleOutline(const BlockPos& holePos, HoleType holeType, float alpha);
public:
	bool isGoodHole(const BlockPos& blockPos);
	HoleType getHoleType(const BlockPos& blockPos);
public:
	HoleESP();
	std::string getModeText() override;
	void onLevelRender() override;
};