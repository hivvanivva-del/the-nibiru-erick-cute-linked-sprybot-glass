#pragma once
#include "../../ModuleBase/Module.h"

class AutoHoleFill : public Module {
public:
	enum class Face : uint8_t {
		Unknown = 0,
		XPositive = 1,
		XNegative = 2,
		ZPositive = 3,
		ZNegative = 4
	};
public:
	std::vector<Actor*> entityList;
	std::vector<Actor*> targetList;
	std::vector<BlockPos> holes;
	std::vector<BlockPos> holeList;
private:
	bool smart = true;
	bool packetPlace = false;
	bool safe = false;
	int blocksPerTick = 4;
	int blockMode = 0;
	float targetDist = 3.f;
	bool raytrace = false;
	bool absoluteRaytrace = false;
	float targetDistWall = 3.f;
	float targetRange = 12.f;
	bool predict = false;
	float safeDistance = 2.f;
	float predictIntensity = 5.f;
	int placeRange = 6;
	int wallRange = 6;
	int placeDelay = 0;
	int proximity = 6;
	bool mobEq = false;
	bool eatStop = false;
	bool render = false;
	int maxHoles = 0;
	UIColor renderColor = UIColor(255, 255, 255, 60);

	std::vector<std::pair<BlockPos, float>> renderList;
private:
	uint8_t getBlock();
	bool isInRenderList(const BlockPos& blockPos);
	bool isValidHole(const BlockPos& blockPos);
	bool canFillHole(const BlockPos& blockPos);
	void getHoleList(LocalPlayer* localPlayer);
	void fillHole(const std::vector<BlockPos>& vector);
	Face getFacing(Actor* actor);
	Face getDirection(const Vec3<float>& a, const BlockPos& b);
public:
	AutoHoleFill();
	virtual void onNormalTick(LocalPlayer* localPlayer) override;
	virtual void onLevelRender() override;
};