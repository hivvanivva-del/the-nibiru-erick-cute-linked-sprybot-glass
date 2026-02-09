#include "InvView.h"

InventoryView::InventoryView() : Module("Inventory", "Display player inventory overlay", Category::RENDER) {
    registerSetting(new SliderSetting<float>("Scale", "Size of the display", &scale, 1.0f, 0.5f, 2.0f));
    registerSetting(new SliderSetting<float>("Opacity", "Background opacity", &opacity, 0.7f, 0.1f, 1.5f));
    registerSetting(new SliderSetting<int>("X", "X Position", &x, 0, 0, 900));
    registerSetting(new SliderSetting<int>("Y", "Y Position", &y, 0, 0, 600));
}

void InventoryView::drawBox(const Vec2<float>& pos, float width, float height) {
    UIColor bg = backgroundColor;
    bg.a = static_cast<uint8_t>(bg.a * opacity);

    Vec4<float> rect(pos.x, pos.y, pos.x + width, pos.y + height);
    MCR::fillRectangle(rect, bg);

    MCR::drawQuad(
        Vec2<float>(rect.x, rect.y), Vec2<float>(rect.z, rect.y),
        Vec2<float>(rect.z, rect.w), Vec2<float>(rect.x, rect.w),
        outlineColor
    );

    float border = 0.5f;
    Colors* colorMod = ModuleManager::getModule<Colors>();
    borderColor = colorMod ? colorMod->getColor() : UIColor(255, 255, 255, 255); 

    MCR::fillRectangle(Vec4<float>(rect.x, rect.y, rect.z, rect.y + border), borderColor);
    MCR::fillRectangle(Vec4<float>(rect.x, rect.w - border, rect.z, rect.w), borderColor);
    MCR::fillRectangle(Vec4<float>(rect.x, rect.y + border, rect.x + border, rect.w - border), borderColor);
    MCR::fillRectangle(Vec4<float>(rect.z - border, rect.y + border, rect.z, rect.w - border), borderColor);
}

void InventoryView::drawInventory() {
    auto player = Game::getLocalPlayer();
    if (!player) return;

    auto playerInv = player->getplayerInventory();
    if (!playerInv) return;

    Container* inv = playerInv->container;
    if (!inv) return;

    const float itemScale = scale;
    const float itemSize = 16.f * itemScale;
    const float padding = 2.f;

    float xStart = static_cast<float>(x);
    float yStart = static_cast<float>(y);
    float xPos = xStart;
    float yPos = yStart;

    float width = itemSize * 9.f + padding * 2.f;
    float height = itemSize * 3.f + padding * 2.f;
    drawBox(Vec2<float>(xStart - padding, yStart - padding), width, height);

    for (int i = 9; i < 36; i++) {
        ItemStack* stack = inv->getItem(i);
        if (stack && stack->isValid()) {
            MCR::drawItem(Vec2<float>(xPos, yPos), stack, itemScale, true);

            if (stack->mCount > 1) {
                float centerX = xPos + itemSize / 2.f;
                float centerY = yPos + itemSize / 2.f;
                float textWidth = (stack->mCount >= 10 ? 8.f : 6.f) * itemScale;
                float textHeight = 8.f * itemScale;

                MCR::drawText(
                    Vec2<float>(centerX - textWidth / 2.f, centerY - textHeight / 2.f),
                    std::to_string(stack->mCount),
                    textColor,
                    itemScale
                );
            }
        }

        xPos += itemSize;
        if ((i + 1) % 9 == 0) {
            xPos = xStart;
            yPos += itemSize;
        }
    }
}

void InventoryView::onMCRender(MinecraftUIRenderContext* ctx) {
    if (!ctx) return;

    Colors* colorMod = ModuleManager::getModule<Colors>();
    if (!colorMod) return;

    outlineColor = colorMod->getColor();
    backgroundColor.a = static_cast<uint8_t>(125 * opacity);
    textColor = UIColor(255, 255, 255, 255);
    borderColor = colorMod->getColor();

    drawInventory();
}