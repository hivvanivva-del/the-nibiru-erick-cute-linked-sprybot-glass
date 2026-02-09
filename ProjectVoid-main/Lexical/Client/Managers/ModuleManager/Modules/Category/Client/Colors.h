#pragma once
#include "../../ModuleBase/Module.h"

class Colors : public Module {
private:
	int colorType = 0;
	UIColor mainColor = UIColor(0, 172, 172, 255);
	UIColor primaryColor = UIColor(0, 172, 172, 255);
	UIColor secondColor = UIColor(0, 172, 172, 255);

	float saturation = 1.f;
	int seperation = 50;
	float seconds = 6.f;
	float brightness = 1.f;
public:
	UIColor getColor(int index = 0) {
		if (colorType == 1) return ColorUtil::getWaveColor(primaryColor, secondColor, index);
		else if (colorType == 2) return ColorUtil::getRainbowColor(seconds, saturation, brightness, index);
		return mainColor;
	}
	int getSeperation() {
		return seperation;
	}
public:
	Colors();
	virtual bool isEnabled() override {
		return true;
	}
	virtual bool isVisible() override {
		return false;
	}
	virtual void setEnabled(bool enabled) override {

	}
};