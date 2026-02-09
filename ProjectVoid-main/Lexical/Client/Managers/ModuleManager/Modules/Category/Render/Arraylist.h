#pragma once
#include "../../ModuleBase/Module.h"

class Arraylist : public Module {
public:
	bool bottom = true;
private:
	bool showModes = true;
	UIColor color = UIColor(0, 172, 172);
	bool rainbow = false;
	int modeColorEnum = 0;
	bool outline = false;
	int offset = 15;
	int opacity = 125;
	int spacing = -1;
	float blurStrength = 5.f;
	static bool sortByLength(Module* lhs, Module* rhs);
public:
	Arraylist();

	void onD2DRender() override;
};