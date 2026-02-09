#pragma once
#include "../../ModuleBase/Module.h"

class TickShift : public Module {
private:
    float currentTPS = 20.f;
    float maxTPS = 60.f;
    float minTPS = 20.f;
    float increment = 5.f;
    bool renderUpdate = true;
    bool pauseInAir = false;
    bool smooth = false;
    bool pauseWhenSneaking = false;

public:
    TickShift();

    void onDisable() override;
    void onClientTick() override;
};
