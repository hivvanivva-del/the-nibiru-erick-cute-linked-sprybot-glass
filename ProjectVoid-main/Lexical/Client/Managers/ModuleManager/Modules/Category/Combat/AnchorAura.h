#pragma once
#include "../../ModuleBase/Module.h"

class AnchorAura : public Module {
public:
	std::vector<Actor*> entityList;
	std::vector<Actor*> targetList;
	std::unordered_map<BlockPos, bool> isCharged;
	std::vector<BlockPos> renderList;
private:
	float placeRange = 6.f;
	float targetRange = 12.f;
	bool eatStop = true;
	bool java = false;
	bool self = false;
	bool hardcode = false;
	bool faceplace = false;
	bool silent = false;
	int proximity = 4;
	float minDamage = 6.f;
	int maxTargets = 0;
	int placeDelay = 1;
	int igniteDelay = 2;
	int detonateDelay = 3;
	UIColor renderColor = UIColor(255, 255, 255, 255);
private:
	std::unordered_map<BlockPos, bool> placed;
	std::unordered_map<BlockPos, bool> charged;
	void clearCache();
	float getExplosionDamage(const BlockPos& blockPos, Actor* actor, float exposure);
	bool isValid(const BlockPos& blockPos, Actor* actor, LocalPlayer* localPlayer);
	BlockPos getPlacePos(Actor* actor, LocalPlayer* localPlayer);
public:
	AnchorAura();
	virtual void onNormalTick(LocalPlayer* localPlayer) override;
	virtual void onLevelRender() override;
	virtual void onDisable() override;
	virtual void onEnable() override;
};