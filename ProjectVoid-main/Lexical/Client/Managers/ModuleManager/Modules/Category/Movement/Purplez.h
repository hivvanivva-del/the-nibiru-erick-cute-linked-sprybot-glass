#pragma once
#include "../../ModuleBase/Module.h"

class Purplez : public Module {
private:
    float spamDelay = 50.f; // Delay in milliseconds

public:
    Purplez();

    void onEnable() override;
    void onDisable() override;
};