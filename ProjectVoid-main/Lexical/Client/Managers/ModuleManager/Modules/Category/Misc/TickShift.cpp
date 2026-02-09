#include "TickShift.h"

TickShift::TickShift()
    : Module("TickShift", "Dynamic TPS shift based on movement", Category::WORLD) {
    registerSetting(new SliderSetting<float>("Max TPS", "Maximum TPS when stationary", &maxTPS, 60.f, 20.f, 200.f));
    registerSetting(new SliderSetting<float>("Min TPS", "Minimum TPS when moving", &minTPS, 20.f, 1.f, 20.f));
    registerSetting(new SliderSetting<float>("Increment", "TPS change per tick", &increment, 5.f, 1.f, 20.f));
    registerSetting(new BoolSetting("Render Update", "Also modify render timer", &renderUpdate, false));
    registerSetting(new BoolSetting("Pause In Air", "Stop charging/discharging while in air", &pauseInAir, false));
    registerSetting(new BoolSetting("Smooth Mode", "Use smaller increments for smoother transitions", &smooth, false));
    registerSetting(new BoolSetting("Pause Sneak", "Pause while sneaking", &pauseWhenSneaking, false));
}

void TickShift::onDisable() {
    Minecraft* mc = Game::clientInstance->minecraft;
    if (mc != nullptr) {
        *mc->minecraftTimer = 20.0f;
        *mc->minecraftRenderTimer = 20.0f;
        currentTPS = 20.f;
    }
}

void TickShift::onClientTick() {
    Minecraft* mc = Game::clientInstance->minecraft;
    LocalPlayer* player = Game::getLocalPlayer();

    if (mc == nullptr || player == nullptr) {
        return;
        this->setEnabled(false);
    }

    bool isMoving = player->stateVector->velocity.x != 0.f || player->stateVector->velocity.z != 0.f;

    if ((pauseInAir && !player->onGround()) || (pauseWhenSneaking && player->getMoveInputComponent()->sneaking)) {
        *mc->minecraftRenderTimer = 20.f;
        *mc->minecraftTimer = 20.f;
        return;
    }

    float tpsStep = smooth ? (increment * 0.5f) : increment;

    if (!isMoving) {
        currentTPS += tpsStep;
        if (currentTPS > maxTPS)
            currentTPS = maxTPS;
        *mc->minecraftRenderTimer = 20.f;
        *mc->minecraftTimer = 20.f;
    }
    else {
        currentTPS -= tpsStep;
        if (currentTPS < minTPS)
            currentTPS = minTPS;
    }
    if (isMoving) {
        *mc->minecraftTimer = currentTPS;
        if (renderUpdate) {
            *mc->minecraftRenderTimer = currentTPS;
        }
    }
}
