#pragma once
#include "../../ModuleBase/Module.h"

class PlayerList : public Module {
public:
    PlayerList();

    // Settings
    float posX = 10.f;
    float posY = 50.f;
    bool showArmor = true;
    bool showDistance = true;
    bool showItems = true;

    virtual void onMCRender(MinecraftUIRenderContext* renderCtx) override;

private:
    void renderPlayerEntry(Actor* player, float& currentY, MinecraftUIRenderContext* renderCtx);
};
