#include "HoleESP.h"

HoleESP::HoleESP() : Module("HoleESP", "Show safe holes for crystal pvp", Category::RENDER) {
	registerSetting(new SliderSetting<int>("HRadius", "Horizontal radius in which to search for holes", &hRadius, 7, 1, 20));
	registerSetting(new SliderSetting<int>("VRadius", "Vertical radius in which to search for holes", &vRadius, 6, 1, 10));
	registerSetting(new BoolSetting("IgnoreOwn", "Ignores rendering the hole you are currently standing in", &ignoreOwn, false));
	registerSetting(new BoolSetting("Fill", "NULL", &fill, true));
	registerSetting(new BoolSetting("Outline", "NULL", &outline, false));
	registerSetting(new ColorSetting("BedrockHole", "Color of bedrock hole", &bHoleColor, UIColor(0, 255, 0, 50)));
	registerSetting(new ColorSetting("ObsidianHole", "Color of obsidian hole", &oHoleColor, UIColor(255, 0, 0, 50)));
	registerSetting(new ColorSetting("MixedHole", "Color of mixed hole", &mHoleColor, UIColor(255, 255, 0, 50)));
}

std::string HoleESP::getModeText() {
	return std::to_string(holeCount);
}

void HoleESP::renderHole(const BlockPos& holePos, HoleType holeType, float alpha) {

	UIColor fillColor;

	switch (holeType) {
	case HoleType::Bedrock: {
		fillColor = bHoleColor;
		break;
	}
	case HoleType::Obsidian: {
		fillColor = oHoleColor;
		break;
	}
	case HoleType::Mixed: {
		fillColor = mHoleColor;
		break;
	}
	}

	Vec3<float> p1 = holePos.CastTo<float>();
	Vec3<float> p2 = holePos.CastTo<float>().add(Vec3<float>(1.f, 0.f, 0.f));
	Vec3<float> p3 = holePos.CastTo<float>().add(Vec3<float>(1.f, 0.f, 1.f));
	Vec3<float> p4 = holePos.CastTo<float>().add(Vec3<float>(0.f, 0.f, 1.f));

	p1 = p1.sub(MCR::origin);
	p2 = p2.sub(MCR::origin);
	p3 = p3.sub(MCR::origin);
	p4 = p4.sub(MCR::origin);

	const mce::Color& color = fillColor.toMCColor();
	MCR::tessellator->color(color.r, color.g, color.b, color.a * alpha);
	MCR::tessellator->vertex(p1.x, p1.y, p1.z);
	MCR::tessellator->vertex(p2.x, p2.y, p2.z);
	MCR::tessellator->vertex(p3.x, p3.y, p3.z);
	MCR::tessellator->vertex(p4.x, p4.y, p4.z);

	MCR::tessellator->vertex(p4.x, p4.y, p4.z);
	MCR::tessellator->vertex(p3.x, p3.y, p3.z);
	MCR::tessellator->vertex(p2.x, p2.y, p2.z);
	MCR::tessellator->vertex(p1.x, p1.y, p1.z);
}

void HoleESP::renderHoleOutline(const BlockPos& holePos, HoleType holeType, float alpha) {

	UIColor outlineColor;

	switch (holeType) {
	case HoleType::Bedrock: {
		outlineColor = bHoleColor;
		break;
	}
	case HoleType::Obsidian: {
		outlineColor = oHoleColor;
		break;
	}
	case HoleType::Mixed: {
		outlineColor = mHoleColor;
		break;
	}
	}

	outlineColor.a = (uint8_t)(outlineColor.a * 1.8f);
	if (outlineColor.a > 255) outlineColor.a = 255;

	Vec3<float> p1 = holePos.CastTo<float>();
	Vec3<float> p2 = holePos.CastTo<float>().add(Vec3<float>(1.f, 0.f, 0.f));
	Vec3<float> p3 = holePos.CastTo<float>().add(Vec3<float>(1.f, 0.f, 1.f));
	Vec3<float> p4 = holePos.CastTo<float>().add(Vec3<float>(0.f, 0.f, 1.f));

	p1 = p1.sub(MCR::origin);
	p2 = p2.sub(MCR::origin);
	p3 = p3.sub(MCR::origin);
	p4 = p4.sub(MCR::origin);

	const mce::Color& color = outlineColor.toMCColor();
	MCR::tessellator->color(color.r, color.g, color.b, color.a * alpha);

	MCR::tessellator->vertex(p1.x, p1.y, p1.z);
	MCR::tessellator->vertex(p2.x, p2.y, p2.z);

	MCR::tessellator->vertex(p2.x, p2.y, p2.z);
	MCR::tessellator->vertex(p3.x, p3.y, p3.z);

	MCR::tessellator->vertex(p3.x, p3.y, p3.z);
	MCR::tessellator->vertex(p4.x, p4.y, p4.z);

	MCR::tessellator->vertex(p4.x, p4.y, p4.z);
	MCR::tessellator->vertex(p1.x, p1.y, p1.z);
}

bool HoleESP::isGoodHole(const BlockPos& blockPos) {
	Block* block = WorldUtil::getBlock(blockPos);
	Block* belowBlock = WorldUtil::getBlock(blockPos.sub(0, 1, 0));
	const int belowId = belowBlock->blockLegacy->blockId;
	if (!(belowId == 7 || belowId == 49)) return false;
	const int blockId = block->blockLegacy->blockId;
	if (!WorldUtil::canBuildOn(blockPos) && blockId != 143) return false;
	const BlockPos& posAbove = blockPos.add(0, 1, 0);
	const BlockPos& posAbove2 = blockPos.add(0, 1, 0);
	if (!WorldUtil::canBuildOn(posAbove)) return false;
	if (!WorldUtil::canBuildOn(posAbove2)) return false;
	return true;
}

HoleESP::HoleType HoleESP::getHoleType(const BlockPos& blockPos) {
	if (!isGoodHole(blockPos)) return HoleType::Invalid;
	int bPoints = 0;
	int oPoints = 0;
	int bPoints2 = 0;
	int oPoints2 = 0;
	Block* belowBlock = WorldUtil::getBlock(blockPos.sub(0, 1, 0));
	const int belowId = belowBlock->blockLegacy->blockId;
	static const BlockPos checkList[4] = { {1, 0, 0}, {-1, 0, 0}, {0, 0, 1}, {0, 0, -1} };
	for (const BlockPos& check : checkList) {
		const BlockPos& pos = blockPos.add(check);
		const int id = WorldUtil::getBlock(pos)->blockLegacy->blockId;
		if (id == 7) bPoints++;
		else if (id == 49) oPoints++;
	}
	const int totalPoints = bPoints + oPoints;
	if (totalPoints == 4) {
		if (bPoints == 4 && belowId == 7) return HoleType::Bedrock;
		else if (oPoints == 4 && belowId == 49) return HoleType::Obsidian;
		else return HoleType::Mixed;
	}
	else if (totalPoints == 3) {
		for (const BlockPos& check : checkList) {
			const BlockPos& pos = blockPos.add(check);
			if (!isGoodHole(pos)) continue;
			for (const BlockPos& check2 : checkList) {
				const BlockPos& pos2 = pos.add(check2);
				const int id = WorldUtil::getBlock(pos2)->blockLegacy->blockId;
				if (id == 7) bPoints2++;
				else if (id == 49) oPoints2++;
			}
		}
		if (bPoints2 + oPoints2 == 3 && totalPoints == 3) {
			if (bPoints2 == 3) return HoleType::Bedrock;
			else if (oPoints2 == 3) return HoleType::Obsidian;
			else return HoleType::Mixed;
		}
	}
	return HoleType::Invalid;
}

void HoleESP::onLevelRender() {
	LocalPlayer* localPlayer = Game::getLocalPlayer();

	const AABB& playerAABB = localPlayer->aabbShape->aabb;
	Vec3<float> feetPos = playerAABB.getCenter();
	feetPos.y = playerAABB.lower.y;
	const BlockPos& currentPos = feetPos.add(Vec3<float>(0.f, 0.5f, 0.f)).floor().CastTo<int>();

	static int xAdd = 0;
	static int yAdd = 0;
	static int zAdd = 0;
	int checkedBlock = 0;

	while (checkedBlock < 1024) {

		int bPoint = 0;
		int oPoint = 0;

		const BlockPos& checkPos = currentPos.add(BlockPos(xAdd, yAdd, zAdd));
		holeMap[checkPos] = std::make_pair(getHoleType(checkPos), 1.f);

		xAdd++;
		if (xAdd > hRadius) {
			xAdd = -hRadius;
			zAdd++;
		}

		if (zAdd > hRadius) {
			zAdd = -hRadius;
			yAdd++;
		}

		if (yAdd > vRadius) {
			yAdd = -vRadius;
		}

		checkedBlock++;
	}

	for (auto it = holeMap.begin(); it != holeMap.end(); ) {
		if (it->second.first == HoleType::Invalid) {
			it = holeMap.erase(it);
			continue;
		}
		if (abs(currentPos.x - it->first.x) > hRadius || abs(currentPos.z - it->first.z) > hRadius || abs(currentPos.y - it->first.y) > vRadius) it->second.second -= D2D::deltaTime * 1.5f;
		if (ignoreOwn && it->first == currentPos) it->second.second -= D2D::deltaTime * 1.5f;
		if (it->second.second <= 0.0f) {
			it = holeMap.erase(it);
			continue;
		}
		it++;
	}
	if (!holeMap.empty()) {
		MCR::setColor(mce::Color(1.f, 1.f, 1.f, 1.f));
		if (fill) {
			MCR::tessellator->begin(VertextFormat::QUAD);
			for (const auto& it : holeMap) {
				renderHole(it.first, it.second.first, it.second.second);
			}
			MeshHelpers::renderMeshImmediately(MCR::screenCtx, MCR::tessellator, MCR::blendMaterial);
		}
		if (outline) {
			MCR::tessellator->begin(VertextFormat::LINE_LIST);
			for (const auto& it : holeMap) {
				renderHoleOutline(it.first, it.second.first, it.second.second);
			}
			MeshHelpers::renderMeshImmediately(MCR::screenCtx, MCR::tessellator, MCR::blendMaterial);
		}
	}

	holeCount = holeMap.size();
}