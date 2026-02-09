#include "PortalESP.h"

PortalESP::PortalESP() : Module("PortalESP", "Renders nether portals in render distance", Category::RENDER) {
	registerSetting(new SliderSetting<int>("Radius", "The more, the slower the module scans for blocks, this is only for horizontal radius, it shouldnt affect fps", &radius, radius, 24, 256));
	registerSetting(new BoolSetting("Tracer", "Draws lines from crosshair", &trace, trace));
	registerSetting(new ColorSetting("Color", "Color of render", &color, color));
	registerSetting(new ColorSetting("Line Color", "Color of render", &lineColor, lineColor));

	registerSetting(new ColorSetting("Tracer Color", "Color of tracers", &tracerColor, tracerColor, false, true, [&]() -> bool {
		return trace;
		}));
}

void PortalESP::onLevelRender() {
	Tessellator* tessellator = MCR::tessellator;

	LocalPlayer* localPlayer = Game::getLocalPlayer();
	const AABB& playerAABB = localPlayer->aabbShape->aabb;

	Vec3<float> feetPos = playerAABB.getCenter(); feetPos.y = playerAABB.lower.y;

	const BlockPos& currentPos = feetPos.add(Vec3<float>(0.f, 0.5f, 0.f)).floor().CastTo<int>();

	static int xAdd = 0;
	static int yAdd = 0;
	static int zAdd = 0;
	int checkedBlock = 0;

	while (checkedBlock < 1024) {
		const BlockPos& checkPos = currentPos.add(BlockPos(xAdd, yAdd, zAdd));
		portalList.insert(checkPos);

		xAdd++;
		if (xAdd > radius) {
			xAdd = -radius;
			zAdd++;
		}

		if (zAdd > radius) {
			zAdd = -radius;
			yAdd++;
		}

		if (yAdd > 64) {
			yAdd = -128;
		}

		checkedBlock++;
	}

	for (auto it = portalList.begin(); it != portalList.end(); ) {
		if (WorldUtil::getBlockLegacy(*it)->blockId != 90) {
			it = portalList.erase(it);
			continue;
		}
		if (abs(currentPos.x - it->x) > 128 || abs(currentPos.z - it->z) > 128 || abs(currentPos.y - it->y) > 128) {
			it = portalList.erase(it);
			continue;
		}
		it++;
	}

	if (!portalList.empty()) {
		for (const auto& pos : portalList) {
			AABB aabb = AABB(pos.CastTo<float>(), pos.CastTo<float>().add(1.f));

			if (trace) {
				float calcYaw = (localPlayer->rotation->presentRot.y + 90.f) * (PI / 180.f);
				float calcPitch = (localPlayer->rotation->presentRot.x) * -(PI / 180.f);
				Vec3<float> moveVec;
				moveVec.x = cos(calcYaw) * cos(calcPitch) * 0.2f;
				moveVec.y = sin(calcPitch) * 0.2f;
				moveVec.z = sin(calcYaw) * cos(calcPitch) * 0.2f;
				Vec3<float> origin = Game::clientInstance->getLevelRenderer()->levelRendererPlayer->realOrigin;
				Vec3<float> start = moveVec;
				MCR::setColor(UIColor(tracerColor.r, tracerColor.g, tracerColor.b, tracerColor.a));
				Vec3<float> end = Vec3<float>(aabb.getCenter().x, aabb.getCenter().y, aabb.getCenter().z).sub(origin);
				tessellator->begin(VertextFormat::LINE_LIST, 2);
				tessellator->vertex(start.x, start.y, start.z);
				tessellator->vertex(end.x, end.y, end.z);
				MeshHelpers::renderMeshImmediately(MCR::screenCtx, tessellator, MCR::blendMaterial);
			}

			MCR::drawBox3dFilled(aabb, UIColor(color.r, color.g, color.b, color.a), UIColor(lineColor.r, lineColor.g, lineColor.b, lineColor.a));
		}
	}
}