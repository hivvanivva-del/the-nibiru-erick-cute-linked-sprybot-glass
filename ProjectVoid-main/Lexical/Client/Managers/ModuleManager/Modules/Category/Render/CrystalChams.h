#pragma once
#include "../../ModuleBase/Module.h"

class CrystalChams : public Module {
public:
	std::unordered_map<int, float> actorAngles;
	std::unordered_map<int, float> actorYOffsets;
	std::unordered_map<int, bool> actorMoveUps;
public:
	UIColor fillColor = UIColor(255, 255, 255, 60);
	UIColor lineColor = UIColor(255, 255, 255, 100);
	float scale = 1.f;
	float rotateSpeed = 1.f;
public:
	CrystalChams();
	virtual void onLevelRender() override;
};