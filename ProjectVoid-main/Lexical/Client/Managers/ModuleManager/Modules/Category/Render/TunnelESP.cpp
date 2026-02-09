#include "TunnelESP.h"

TunnelESP::TunnelESP() : Module("TunnelESP", "Renders tunnels", Category::RENDER) {
	registerSetting(new ColorSetting("Color", "Color", &color, color));
}

bool TunnelESP::isTunnel(const BlockPos& blockPos) {
	static std::vector<BlockPos> xTunnel = { {1, 0, 0}, {-1, 0, 0}, {1, 1, 0}, {-1, 1, 0} };
	static std::vector<BlockPos> zTunnel = { {1, 0, 0}, {-1, 0, 0}, {1, 1, 0}, {-1, 1, 0} };

	if (!WorldUtil::getBlockLegacy(blockPos.add(0, -1, 0))->isSolid()) {
		return false;
	}
	if (!WorldUtil::getBlockLegacy(blockPos.add(0, 2, 0))->isSolid()) {
		return false;
	}

	if (WorldUtil::getBlockLegacy(blockPos)->isSolid()) {
		return false;
	}
	if (WorldUtil::getBlockLegacy(blockPos.add(0, 1, 0))->isSolid()) {
		return false;
	}

	int check = 0;
	for (BlockPos tun : xTunnel) {
		BlockPos pos = blockPos.add(tun);
		if (!WorldUtil::getBlockLegacy(pos)->isSolid()) {
			check++;
			break;
		}
	}
	for (BlockPos tun : zTunnel) {
		BlockPos pos = blockPos.add(tun);
		if (!WorldUtil::getBlockLegacy(pos)->isSolid()) {
			check++;
			break;
		}
	}

	if (check != 2) {
		return true;
	}

	return false;
}

void TunnelESP::onLevelRender() {
	static constexpr int RADIUS = 128;

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
		tunnelList.insert(checkPos);

		xAdd++;
		if (xAdd > RADIUS) {
			xAdd = -RADIUS;
			zAdd++;
		}

		if (zAdd > RADIUS) {
			zAdd = -RADIUS;
			yAdd++;
		}

		if (yAdd > 64) {
			yAdd = -128;
		}

		checkedBlock++;
	}

	for (auto it = tunnelList.begin(); it != tunnelList.end(); ) {
		if (!isTunnel(*it)) {
			it = tunnelList.erase(it);
			continue;
		}
		if (abs(currentPos.x - it->x) > 128 || abs(currentPos.z - it->z) > 128 || abs(currentPos.y - it->y) > 128) {
			it = tunnelList.erase(it);
			continue;
		}
		it++;
	}

	if (!tunnelList.empty()) {
		for (const auto& pos : tunnelList) {
			AABB aabb = AABB(pos.CastTo<float>(), pos.CastTo<float>().add(1.f, 2.f, 1.f));
			MCR::drawBox3dFilled(aabb, UIColor(color.r, color.g, color.b, color.a), UIColor(color.r, color.g, color.b, color.a - 20));
		}
	}
}