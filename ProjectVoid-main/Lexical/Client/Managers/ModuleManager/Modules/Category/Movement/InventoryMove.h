#pragma once
#include "../../ModuleBase/Module.h"

class InventoryMove : public Module {
public:
	InventoryMove() : Module("InventoryMove", "Move when ur in inventory/container", Category::MOVEMENT) {

	}
	void onNormalTick(LocalPlayer* localPlayer) override;
};