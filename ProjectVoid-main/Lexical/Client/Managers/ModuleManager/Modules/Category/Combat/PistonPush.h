#pragma once
#include "../../ModuleBase/Module.h"

class PistonPush : public Module {
public:
	enum class RedstoneType : uint8_t {
		RedstoneBlock = 0,
		RedstoneTorch = 1,
		None = 2
	};
public:
	float targetRange = 6.f;
	int pistonDelay = 0;
	int redstoneDelay = 0;
	bool airplace = false;
	bool packetPlace = false;
	bool java = false;
	bool self = false;
	UIColor pistonColor = UIColor(255, 255, 255, 60);
	UIColor redstoneColor = UIColor(255, 255, 255, 60);
private:
	std::vector<Actor*> actorList;
	std::vector<Actor*> targetList;
private:
	int yaw = 0;
	BlockPos getOffset(LocalPlayer* localPlayer);
	std::pair<BlockPos, BlockPos> getPlacePos(Actor& actor);
	bool isPlaceValid(const BlockPos& blockPos, const int targetBlockId);
	bool canPlaceBlock(const BlockPos& blockPos, bool airplace, const int targetBlockId);
	bool placeBlock(const BlockPos& blockPos, bool airplace, bool packetPlace);
	RedstoneType getRedstoneType();
public:
	PistonPush();
	virtual void onNormalTick(LocalPlayer* localPlayer) override;
	virtual void onLevelRender() override;
};