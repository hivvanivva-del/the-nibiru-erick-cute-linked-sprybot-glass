#include "Chams.h"
#include "../../../ModuleManager.h"
#include <string>

bool showSelff = false;
UIColor boxColor = UIColor(255, 255, 255);
PlayerChams::PlayerChams()
    : Module("PlayerCham", "Render custom box per body part on players", Category::RENDER) {
    registerSetting(new ColorSetting("Color", "NULL", &boxColor, UIColor(255, 255, 255), false));
    registerSetting(new BoolSetting("Self", "Render yourself", &showSelff, false));
}

static AABB makeBoxAt(const Vec3<float>& center, const Vec3<float>& halfSize) {
    Vec3<float> min{ center.x - halfSize.x, center.y - halfSize.y, center.z - halfSize.z };
    Vec3<float> max{ center.x + halfSize.x, center.y + halfSize.y, center.z + halfSize.z };
    return AABB(min, max);
}

void PlayerChams::onLevelRender() {
    LocalPlayer* lp = Game::getLocalPlayer();
    if (!lp || !lp->level) return;

    const auto& entities = lp->level->getRuntimeActorList();
    static Colors* colorMod = ModuleManager::getModule<Colors>();

    float yOffset = -1.8f;

    for (Actor* entity : entities) {
        if (!entity) continue;

        auto actorTypeComp = entity->getActorTypeComponent();
        if (!actorTypeComp || actorTypeComp->id != 319) continue;

        if (entity == lp && !showSelff) continue;
        if (!entity->getNameTag()) continue;

        boxColor.a = 50;
        UIColor lineColor(boxColor.r, boxColor.g, boxColor.b, 150);

        Vec3<float> pos = entity->getPos();

        // Head
        Vec3<float> headOffset = { 0.f, 1.85f + yOffset, 0.f };
        Vec3<float> headHalfSize = { 0.18f, 0.25f, 0.25f };
        Vec3<float> headPos{ pos.x + headOffset.x, pos.y + headOffset.y, pos.z + headOffset.z };
        MCR::drawBox3dFilled(makeBoxAt(headPos, headHalfSize), boxColor, lineColor, 1.0f);

        // Chest
        Vec3<float> chestOffset = { 0.f, 1.275f + yOffset, 0.f };
        Vec3<float> chestHalfSize = { 0.25f, 0.275f, 0.15f };
        Vec3<float> chestPos{ pos.x + chestOffset.x, pos.y + chestOffset.y, pos.z + chestOffset.z };
        MCR::drawBox3dFilled(makeBoxAt(chestPos, chestHalfSize), boxColor, lineColor, 1.0f);

        // Right Arm
        Vec3<float> rArmOffset = { 0.425f, 1.225f + yOffset, 0.f };
        Vec3<float> armHalfSize = { 0.125f, 0.375f, 0.15f };
        Vec3<float> rArmPos{ pos.x + rArmOffset.x, pos.y + rArmOffset.y, pos.z + rArmOffset.z };
        MCR::drawBox3dFilled(makeBoxAt(rArmPos, armHalfSize), boxColor, lineColor, 1.0f);

        // Left Arm
        Vec3<float> lArmOffset = { -0.425f, 1.225f + yOffset, 0.f };
        Vec3<float> lArmPos{ pos.x + lArmOffset.x, pos.y + lArmOffset.y, pos.z + lArmOffset.z };
        MCR::drawBox3dFilled(makeBoxAt(lArmPos, armHalfSize), boxColor, lineColor, 1.0f);

        // Right Leg
        Vec3<float> rLegOffset = { 0.14f, 0.575f + yOffset, 0.f };
        Vec3<float> legHalfSize = { 0.11f, 0.375f, 0.14f };
        Vec3<float> rLegPos{ pos.x + rLegOffset.x, pos.y + rLegOffset.y, pos.z + rLegOffset.z };
        MCR::drawBox3dFilled(makeBoxAt(rLegPos, legHalfSize), boxColor, lineColor, 1.0f);

        // Left Leg
        Vec3<float> lLegOffset = { -0.14f, 0.575f + yOffset, 0.f };
        Vec3<float> lLegPos{ pos.x + lLegOffset.x, pos.y + lLegOffset.y, pos.z + lLegOffset.z };
        MCR::drawBox3dFilled(makeBoxAt(lLegPos, legHalfSize), boxColor, lineColor, 1.0f);
    }
}