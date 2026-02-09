#include "Stats.h"
#include "../../../ModuleManager.h"
#include <vector>
#include <string>
#include <utility>

Stats::Stats() : Module("CombatGearInfo", "Shows basic player stats", Category::RENDER) {
    registerSetting(new SliderSetting<int>("PosX", "X Position", &posX, posX, 0, 500));
    registerSetting(new SliderSetting<int>("PosY", "Y Position", &posY, posY, 0, 300));
    registerSetting(new SliderSetting<float>("Scale", "Text Scale", &scale, scale, 0.1f, 1.0f));

    //registerSetting(new BoolSetting("Show HP", "Toggle HP display", &showHP, true));
    registerSetting(new BoolSetting("Show Crystals", "Toggle Crystals display", &showCrystals, true));
    registerSetting(new BoolSetting("Show Obsidian", "Toggle Obsidian display", &showObsidian, true));
    registerSetting(new BoolSetting("Show Gapples", "Toggle Gapples display", &showGapples, true));
    registerSetting(new BoolSetting("Show Pots", "Toggle Pots display", &showPots, true));
    registerSetting(new BoolSetting("Show Totems", "Toggle Totems display", &showTotems, true));
}

void Stats::onD2DRender() {
    auto* localPlayer = Game::getLocalPlayer();
    if (!localPlayer || !localPlayer->playerInventory || !localPlayer->playerInventory->container)
        return;

    Vec2<float> pos(static_cast<float>(posX), static_cast<float>(posY));
    Container* inv = localPlayer->playerInventory->container;

    int crystalCount = 0;
    int potCount = 0;
    int gappleCount = 0;
    int totemCount = 0;
    int obsidianCount = 0;

    for (int i = 0; i < 36; i++) {
        ItemStack* stack = inv->getItem(i);
        if (stack && stack->isValid() && stack->item) {
            int itemId = stack->item->itemId;
            if (itemId == 758) {
                crystalCount += stack->mCount;
            }
            else if (itemId == 577 || itemId == 435) { 
                potCount += stack->mCount;
            }
            else if (itemId == 263) {
                gappleCount += stack->mCount;
            }
            else if (itemId == 584) {
                totemCount += stack->mCount;
            }
            else if (itemId == 49) {
                obsidianCount += stack->mCount;
            }
        }
    }

    
    
 

    

    auto getColor = [](int count) -> UIColor {
        if (count <= 2) return UIColor(255, 0, 0, 255);
        else if (count <= 5) return UIColor(255, 255, 0, 255);
        else return UIColor(0, 255, 0, 255);
        };

    auto getCrystalColor = [](int count) -> UIColor {
        if (count <= 10) return UIColor(255, 0, 0, 255);
        else if (count <= 25) return UIColor(255, 255, 0, 255);
        else return UIColor(0, 255, 0, 255);
        };

    float textSize = 4.0f * scale;
    float lineSpacing = textSize + 30.0f;
    float yOffset = 0.f;

    float baseOffsetX = 260.0f * scale;
    float potsOffsetX = baseOffsetX;
    float othersOffsetX = baseOffsetX + 22.0f;
    float gapplesOffsetX = baseOffsetX + 30.f;
    float crystalOffsetX = othersOffsetX + 12.0f; 
    float hpOffsetX = 200.0f * scale; 

    auto drawLine = [&](const std::string& label, int value, const UIColor& labelColor, const UIColor& valueColor, float valueOffset) {
        D2D::drawText({ pos.x, pos.y + yOffset }, label + ":", labelColor, textSize);
        D2D::drawText({ pos.x + valueOffset, pos.y + yOffset }, std::to_string(value), valueColor, textSize);
        yOffset += lineSpacing;
        };

    
    if (showPots)
        drawLine("Potions", potCount, UIColor(160, 210, 80, 255), UIColor(160, 210, 80, 255), othersOffsetX);

    if (showTotems)
        drawLine("Totems", totemCount, UIColor(230, 250, 170, 255), UIColor(230, 250, 170, 255), othersOffsetX);

    if (showGapples)
        drawLine("Gapples", gappleCount, UIColor(160, 160, 70, 255), UIColor(160, 160, 70, 255), gapplesOffsetX);


    if (showCrystals)
        drawLine("Crystals", crystalCount, UIColor(170, 20, 190, 255), UIColor(170, 20, 190, 255), crystalOffsetX);
    
    if (showObsidian)
        drawLine("Obsidian", crystalCount, UIColor(0, 0, 0, 255), UIColor(0, 0, 0, 255), crystalOffsetX);
}
