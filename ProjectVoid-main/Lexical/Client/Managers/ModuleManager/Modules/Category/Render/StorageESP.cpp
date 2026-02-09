#include "StorageESP.h"

StorageESP::StorageESP() : Module("StorageESP", "Renders storages in render distance", Category::RENDER) {
	registerSetting(new SliderSetting<int>("Radius", "The more, the slower the module scans for blocks, this is only for horizontal radius, it shouldnt affect fps", &radius, radius, 24, 256));
	registerSetting(new BoolSetting("Tracer", "Draws lines from crosshair", &trace, trace));
	registerSetting(new SliderSetting<int>("Alpha", "Opacity of render", &alpha, alpha, 0, 255));
	registerSetting(new SliderSetting<int>("Tracer Alpha", "Opacity of render", &tracerAlpha, tracerAlpha, 0, 255, [&]() -> bool {
		return trace;
		}));
}

UIColor StorageESP::getColor(const BlockPos& blockPos) {
	int blockId = WorldUtil::getBlockLegacy(blockPos)->blockId;
	
	if (blockId == 54) { // chest
		return UIColor(255, 165, 0);
	}
	else if (blockId == 130) { // ender chest
		return UIColor(255, 0, 255);
	}
	else if (blockId == 205 || blockId == 218) { // shulker
		return UIColor(255, 255, 255);
	}
	else if (blockId == 458) { // barrel
		return UIColor(155, 75, 0);
	}
	else if (blockId == 154) { // hopper
		return UIColor(128, 128, 128);
	}
	else if (blockId == 61 || blockId == 451 || blockId == 453) { // furnace
		return UIColor(70, 70, 70);
	}
	return UIColor(0, 0, 0, 0);
}

void StorageESP::onLevelRender() {
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
		blockList[checkPos] = getColor(checkPos);

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

	for (auto it = blockList.begin(); it != blockList.end(); ) {
		if (it->second == UIColor(0, 0, 0, 0)) {
			it = blockList.erase(it);
			continue;
		}
		if (abs(currentPos.x - it->first.x) > 128 || abs(currentPos.z - it->first.z) > 128 || abs(currentPos.y - it->first.y) > 128) {
			it = blockList.erase(it);
			continue;
		}
		if (getColor(it->first) == UIColor(0, 0, 0, 0)) {
			it = blockList.erase(it);
			continue;
		}
		it++;
	}

	if (!blockList.empty()) {
		for (const auto& pos : blockList) {
			AABB aabb = AABB(pos.first.CastTo<float>(), pos.first.CastTo<float>().add(1.f));

			if (trace) {
				float calcYaw = (localPlayer->rotation->presentRot.y + 90.f) * (PI / 180.f);
				float calcPitch = (localPlayer->rotation->presentRot.x) * -(PI / 180.f);
				Vec3<float> moveVec;
				moveVec.x = cos(calcYaw) * cos(calcPitch) * 0.2f;
				moveVec.y = sin(calcPitch) * 0.2f;
				moveVec.z = sin(calcYaw) * cos(calcPitch) * 0.2f;
				Vec3<float> origin = Game::clientInstance->getLevelRenderer()->levelRendererPlayer->realOrigin;
				Vec3<float> start = moveVec;
				MCR::setColor(UIColor(pos.second.r, pos.second.g, pos.second.b, tracerAlpha));
				Vec3<float> end = Vec3<float>(aabb.getCenter().x, aabb.getCenter().y, aabb.getCenter().z).sub(origin);
				tessellator->begin(VertextFormat::LINE_LIST, 2);
				tessellator->vertex(start.x, start.y, start.z);
				tessellator->vertex(end.x, end.y, end.z);
				MeshHelpers::renderMeshImmediately(MCR::screenCtx, tessellator, MCR::blendMaterial);
			}

			MCR::drawBox3dFilled(aabb, UIColor(pos.second.r, pos.second.g, pos.second.b, alpha), UIColor(pos.second.r, pos.second.g, pos.second.b, alpha - 20));
		}
	}
}