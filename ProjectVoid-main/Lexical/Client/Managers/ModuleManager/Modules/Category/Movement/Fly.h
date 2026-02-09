#pragma once
#include "../../ModuleBase/Module.h"

class Fly : public Module {
private:
    float hSpeed = 1.f;
    float currentSpeed = 1.f;
    float vSpeed = 0.5f;
    float Glide = -0.02f;
    bool bypass = false;
    bool speedReduced = false;
    float purplezSpamTimer = 0.f; // Timer for Purplez spamming
    bool isPurplezForward = false; // State for Purplez 'W' key simulation
    float minSpamDelay = 50.f; // Minimum delay for Purplez 'W' spamming (ms)
    float maxSpamDelay = 100.f; // Maximum delay for Purplez 'W' spamming (ms)
    float currentSpamDelay = 50.f; // Current random delay for Purplez spamming (ms)

public:
    Fly();
    void onNormalTick(LocalPlayer* localPlayer) override;

    void reduceSpeed();
    void restoreSpeed();
    bool isBypassEnabled() const { return bypass; }
};