#pragma once
#include "../../ModuleBase/Module.h"

class KillauraTRI : public Module {
public:
    KillauraTRI();

    void onNormalTick(LocalPlayer* localPlayer) override;
    void onUpdateRotation(LocalPlayer* localPlayer) override;
};