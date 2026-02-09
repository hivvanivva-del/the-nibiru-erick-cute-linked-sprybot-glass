#pragma once
#include "../../ModuleBase/Module.h"

class FreeCam : public Module {
private:
    float Speed = 0.5f;
public:
    FreeCam();
    bool isCancelMovePacket;
    void onNormalTick(LocalPlayer* localPlayer) override;
    void onSendPacket(Packet* packet, bool& cancel) override;
    void onEnable() override;
    void onDisable() override;
    Vec3<float> spoofedPos = { 0, 0, 0 };
};