#include "PlayerList.h"
#include "../../../ModuleManager.h"

PlayerList::PlayerList() : Module("PlayerList", "Shows a list of players in the world", Category::RENDER) {
    registerSetting(new SliderSetting<float>("X", "X position", &posX, 10.f, 0.f, 2000.f));
    registerSetting(new SliderSetting<float>("Y", "Y position", &posY, 50.f, 0.f, 2000.f));
    registerSetting(new BoolSetting("ShowArmor", "Show player armor", &showArmor, true));
    registerSetting(new BoolSetting("Distance", "Show distance to players", &showDistance, true));
    registerSetting(new BoolSetting("ShowItems", "Show player held items", &showItems, true));
}

void PlayerList::onMCRender(MinecraftUIRenderContext* renderCtx) {
    LocalPlayer* localPlayer = Game::getLocalPlayer();
    if (!localPlayer || !localPlayer->level) return;

    float currentY = posY;
    auto actorList = localPlayer->level->getRuntimeActorList();

    for (Actor* actor : actorList) {
        if (!actor || actor == localPlayer) continue;

        // Filter for players only (ID 319 is player)
        auto typeComp = actor->getActorTypeComponent();
        if (!typeComp || typeComp->id != 319) continue;

        renderPlayerEntry(actor, currentY, renderCtx);
    }
}

void PlayerList::renderPlayerEntry(Actor* player, float& currentY, MinecraftUIRenderContext* renderCtx) {
    LocalPlayer* localPlayer = Game::getLocalPlayer();
    if (!player || !localPlayer) return;

    // 1. Data Gathering
    std::string name = player->getNameTag2();
    if (name.empty()) return;

    // Remove formatting codes if any (optional but good for clean look)
    // name = StringUtil::removeColorCodes(name); 

    // Hurt time for indicator
    int hurtTime = 0;
    uintptr_t playerAddr = (uintptr_t)player;
    uint32_t** hurtTimePtrPtr = (uint32_t**)(playerAddr + 0x5A8);
    if (hurtTimePtrPtr && *hurtTimePtrPtr) {
        hurtTime = **hurtTimePtrPtr;
    }

    // 2. Rendering
    float currentX = posX;
    float textSize = 1.0f;
    float textHeight = MCR::getTextHeight(textSize);
    float rowHeight = textHeight + 6.f;
    float indicatorSize = 8.f;
    
    // Calculate vertical centering for the indicator box relative to the text
    float indicatorY = currentY + (textHeight - indicatorSize) / 2.f;

    // Indicator Box
    UIColor indicatorColor = (hurtTime > 0) ? UIColor(255, 50, 50) : UIColor(50, 255, 50);
    MCR::fillRectangle(Vec4<float>(currentX, indicatorY, currentX + indicatorSize, indicatorY + indicatorSize), indicatorColor);
    currentX += indicatorSize + 4.f;

    // Name
    MCR::drawText(Vec2<float>(currentX, currentY), name, UIColor(255, 255, 255), textSize);
    currentX += MCR::getTextWidth(name, textSize) + 4.f;

    // Distance
    if (showDistance) {
        float dist = localPlayer->getPos().dist(player->getPos());
        char distBuf[32];
        snprintf(distBuf, sizeof(distBuf), "[%.1f]", dist);
        std::string distStr = distBuf;
        MCR::drawText(Vec2<float>(currentX, currentY), distStr, UIColor(200, 200, 200), textSize);
        currentX += MCR::getTextWidth(distStr, textSize) + 8.f;
    }

    // Items (Held and Offhand)
    if (showItems) {
        ItemStack* mainHand = player->getCarriedItem();
        ItemStack* offHand = player->getOffhandSlot();

        if (mainHand && mainHand->isValid()) {
            MCR::drawItem(Vec2<float>(currentX, currentY - 2), mainHand, 0.75f, true);
            currentX += 14.f;
        }
        if (offHand && offHand->isValid()) {
            MCR::drawItem(Vec2<float>(currentX, currentY - 2), offHand, 0.75f, true);
            currentX += 14.f;
        }
        currentX += 4.f;
    }

    // Armor
    if (showArmor) {
        for (int i = 0; i < 4; i++) {
            ItemStack* armor = player->getArmor(i);
            if (armor && armor->isValid()) {
                MCR::drawItem(Vec2<float>(currentX, currentY - 2), armor, 0.75f, true);
                currentX += 14.f;
            }
        }
    }

    currentY += rowHeight;
}
