#pragma once
#include "../../ModuleBase/Module.h"

class Tint : public Module {
public:
	UIColor color = UIColor(0, 0, 0, 30);
	Tint() : Module("Tint", "Tints your screen oooohhhh", Category::RENDER) {
		registerSetting(new ColorSetting("Color", "NULL", &color, color));
	}

	virtual void onD2DRender() override {
		D2D::fillRectangle(Vec4<float>(0, 0, D2D::getWindowSize().x, D2D::getWindowSize().y), color);
	}
};