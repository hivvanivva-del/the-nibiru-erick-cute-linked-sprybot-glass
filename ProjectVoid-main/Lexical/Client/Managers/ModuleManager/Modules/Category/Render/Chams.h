#pragma once
#include "../../ModuleBase/Module.h"

class PlayerChams : public Module {
public:
	PlayerChams();
	void onLevelRender() override;
};