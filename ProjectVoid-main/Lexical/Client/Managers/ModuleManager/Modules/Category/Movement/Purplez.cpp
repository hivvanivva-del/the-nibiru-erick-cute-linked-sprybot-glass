#include "Purplez.h"
#include "../../../ModuleManager.h"

Purplez::Purplez() : Module("Purplez", "Outrotate ur enemy", Category::MOVEMENT) {
    registerSetting(new SliderSetting<float>("SpamDelay", "Delay between W key toggles (ms)", &spamDelay, 50.f, 10.f, 500.f)); //uncomottrd by dest
}

void Purplez::onEnable() {
    // No specific actions needed on enable
}

void Purplez::onDisable() {
    // No specific actions needed on disable
}