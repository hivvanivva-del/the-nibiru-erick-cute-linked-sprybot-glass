#pragma once
#include "../../ModuleBase/Module.h"

class Stats : public Module {
public:
    Stats();
    void onD2DRender() override;

private:
    int posX = 10;
    int posY = 300;
    int opacity = 120;
    float scale = 0.3f;
    bool showHP = true;
    bool showCrystals = true;
    bool showGapples = true;
    bool showPots = true;
    bool showTotems = true;
    bool showObsidian = true;
};
