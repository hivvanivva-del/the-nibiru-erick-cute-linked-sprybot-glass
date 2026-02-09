#include "Fly.h"
#include "../../../ModuleManager.h"
#include "Purplez.h"
#include <chrono>
#include <random>

Fly::Fly() : Module("Fly", "Fly like superman", Category::MOVEMENT) {
    registerSetting(new SliderSetting<float>("H-Speed", "Horizontal speed", &hSpeed, 1.f, 0.2f, 3.f));
    registerSetting(new SliderSetting<float>("V-Speed", "Vertical speed", &vSpeed, 0.95f, 0.2f, 2.f));
    registerSetting(new SliderSetting<float>("Glide", "Value that how much u want to go down", &Glide, -0.02f, -0.3f, 0.f));
    registerSetting(new BoolSetting("Bypass", "Enable bypass for IGN", &bypass, false));
    registerSetting(new SliderSetting<float>("Min Delay", "Purplez Min Delay", &minSpamDelay, 50.f, 10.f, 500.f));
    registerSetting(new SliderSetting<float>("Max Delay", "Purplez Max Delay", &maxSpamDelay, 100.f, 10.f, 500.f));
}

void Fly::reduceSpeed() {
    if (bypass && !speedReduced) {
        speedReduced = true;
    }
}

void Fly::restoreSpeed() {
    speedReduced = false;
}

void Fly::onNormalTick(LocalPlayer* localPlayer) {
    static auto lastTime = std::chrono::steady_clock::now();
    auto currentTime = std::chrono::steady_clock::now();
    float deltaTime = std::chrono::duration<float, std::milli>(currentTime - lastTime).count();
    lastTime = currentTime;

    Vec3<float>& velocity = localPlayer->stateVector->velocity;
    velocity = Vec3<float>(0.f, 0.f, 0.f);
    velocity.y += Glide;

    
    Purplez* purplez = ModuleManager::getModule<Purplez>();
    bool isPurplezEnabled = purplez && purplez->isEnabled();

    if (Game::canUseMoveKeys()) {
        float yaw = localPlayer->rotation->presentRot.y;
        bool isForward = Game::isKeyDown('W');
        bool isLeft = Game::isKeyDown('A');
        bool isBackward = Game::isKeyDown('S');
        bool isRight = Game::isKeyDown('D');
        bool isUp = Game::isKeyDown(VK_SPACE);
        bool isDown = Game::isKeyDown(VK_SHIFT);
        Vec2<int> moveValue;

        
        if (isRight) moveValue.x += 1;
        if (isLeft) moveValue.x -= 1;

        
        if (isPurplezEnabled && isForward) {
            purplezSpamTimer += deltaTime; 
            if (purplezSpamTimer >= currentSpamDelay) {
                isPurplezForward = !isPurplezForward; 
                purplezSpamTimer = 0; 
                
                std::random_device rd;
                std::mt19937 gen(rd());
                std::uniform_real_distribution<float> dis(minSpamDelay, maxSpamDelay);
                currentSpamDelay = dis(gen);
            }
            if (isPurplezForward) {
                moveValue.y += 1; // Simulate 'W' key press
            }
        }
        else {
            // Normal forward/backward key handling
            if (isForward) moveValue.y += 1;
            if (isBackward) moveValue.y -= 1;
        }

        if (isUp)
            velocity.y += vSpeed;
        if (isDown)
            velocity.y -= vSpeed;

        float angleRad = std::atan2(moveValue.x, moveValue.y);
        float angleDeg = angleRad * (180.f / PI);
        yaw += angleDeg;
        Vec3<float> moveVec = Vec3<float>(0.f, velocity.y, 0.f);
        float currentHSpeed = hSpeed;

        if (bypass && speedReduced) {
            currentHSpeed = 1.0f;
        }
        else if (bypass && isUp) {
            currentHSpeed = 1.14f;
        }

        if ((moveValue.x != 0) || (moveValue.y != 0)) {
            float calcYaw = (yaw + 90.f) * (PI / 180.f);
            moveVec.x = cos(calcYaw) * currentHSpeed;
            moveVec.z = sin(calcYaw) * currentHSpeed;
        }
        localPlayer->lerpMotion(moveVec);
    }
}