#pragma once
#include "../../ModuleBase/Module.h"
#include "../../../ModuleManager.h"

class InventoryView : public Module {
private:
    float scale = 1.0f;
    float opacity = 0.7f;
    int x = 20;
    int y = 100;

    UIColor backgroundColor = UIColor(0, 0, 0, 125);
    UIColor outlineColor = UIColor(0, 0, 0, 255);
    UIColor borderColor;
    UIColor textColor = UIColor(255, 255, 255, 255);

    void drawBox(const Vec2<float>& pos, float width, float height);
    void drawInventory();

public:
    InventoryView();

    void onMCRender(MinecraftUIRenderContext* ctx) override;
};