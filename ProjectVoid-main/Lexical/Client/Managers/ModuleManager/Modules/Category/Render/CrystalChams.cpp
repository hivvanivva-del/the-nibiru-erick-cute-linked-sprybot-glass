#include "CrystalChams.h"

CrystalChams::CrystalChams() : Module("CrystalChams", "Chams for end crystal", Category::RENDER) {
	registerSetting(new ColorSetting("Fill Color", "Color of the chams", &fillColor, fillColor));
	registerSetting(new ColorSetting("Line Color", "Color of the chams", &lineColor, lineColor));
	registerSetting(new SliderSetting<float>("Speed", "Rotating speed", &rotateSpeed, rotateSpeed, 0.f, 2.f));
	registerSetting(new SliderSetting<float>("Scale", "Crystal scale", &scale, scale, 0.f, 2.f));
}

void CrystalChams::onLevelRender() {
	LocalPlayer* localPlayer = Game::getLocalPlayer();
	for (Actor* actor : localPlayer->level->getRuntimeActorList()) {
		if (!actor) continue;
		if (actor->getActorTypeComponent()->id != 71) continue;
		int rId = actor->getRuntimeIDComponent()->runtimeId.id;
		if (actorAngles.find(rId) == actorAngles.end()) {
			actorAngles[rId] = Math::randomFloat(0.f, 360.f);
			actorYOffsets[rId] = Math::randomFloat(0.0f, 0.35f);
			actorMoveUps[rId] = false;
		}

		float& angle = actorAngles[rId];

		angle = fmod(angle + (rotateSpeed * MCR::deltaTime), 360.f);

		float& yOffset = actorYOffsets[rId];
		bool& moveUp = actorMoveUps[rId];

		if (yOffset >= 1.f) moveUp = false;
		else if (yOffset <= 0.f) moveUp = true;
		
		float upSpeed = 1.35f * MCR::deltaTime;
		yOffset += moveUp ? upSpeed : -upSpeed;

		Vec3<float> crystalPos = actor->getEyePos().add(0.f, 0.4f + yOffset, 0.f);
		AABB aabb = AABB(crystalPos.sub(0.5f), crystalPos.add(0.5f));

		MCR::drawBox3dFilled(aabb, fillColor, lineColor, 0.65f * scale, MCR::BoxRotation(Vec3<float>(1.f, 1.f, 0.f), angle));
		MCR::drawBox3dFilled(aabb, fillColor, lineColor, 0.90f * scale, MCR::BoxRotation(Vec3<float>(1.f, 1.f, 0.f), angle - 5.f));
		MCR::drawBox3dFilled(aabb, fillColor, lineColor, 1.f * scale, MCR::BoxRotation(Vec3<float>(1.f, 1.f, 0.f), angle - 10.f));
	}
}