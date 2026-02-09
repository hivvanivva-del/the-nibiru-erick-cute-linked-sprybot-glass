#pragma once
#include "../../ModuleBase/Module.h"

class AutoMapart : public Module {
public:
	BlockPos startPos = BlockPos(0, -32000, 0);
	std::unordered_map<BlockPos, std::optional<UIColor>> mapArt;
	std::vector<unsigned char> image;
	std::string mapartName = "image";
private:
	bool autoBuild = false;
	int buildRange = 5;
	int blocksPerTick = 10;
	int placeDelay = 0;
	int ratio = 100;
	bool silent = false;
	bool swap = true;
	bool mobEquipment = false;
	bool noRender = false;
protected:
	float getDifference(const UIColor& c1, const UIColor& c2);
	bool loadImage(std::vector<unsigned char>& image, const std::string& filename, int& x, int& y);
	uint8_t getItem(const UIColor& color);
	UIColor getItemColor(ItemStack* item);
public:
	AutoMapart();
	virtual void onNormalTick(LocalPlayer* lp) override;
	virtual void onLevelRender() override;
	virtual void onEnable() override;
};