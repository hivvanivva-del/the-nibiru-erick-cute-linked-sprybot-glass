#include "NewChunks.h"

NewChunks::NewChunks() : Module("NewChunks", "Highlights newly generated chunks", Category::WORLD) {
}

std::string NewChunks::getModeText() {
	return std::to_string(chunkCount);
}

void NewChunks::renderNewChunk(const ChunkPos& chunkPos) {

	static constexpr float yHeight = 30.f;
	//Too lazy to make option for it

	Vec3<float> point3D[4];
	point3D[0] = Vec3<float>((float)chunkPos.x, yHeight, (float)chunkPos.z);
	point3D[1] = Vec3<float>((float)chunkPos.x + 16.f, yHeight, (float)chunkPos.z);
	point3D[2] = Vec3<float>((float)chunkPos.x + 16.f, yHeight, (float)chunkPos.z + 16.f);
	point3D[3] = Vec3<float>((float)chunkPos.x, yHeight, (float)chunkPos.z + 16.f);

	Vec2<float> point2D[4];
	for (int i = 0; i < 4; i++) {
		if (!MCR::worldToScreen(point3D[i], point2D[i]))
			return;
	}

	for (int i = 0; i < 4; i++) {
		MCR::tessellator->vertex(point2D[i].x, point2D[i].y, 0.f);
	}

	for (int i = 3; i >= 0; i--) {
		MCR::tessellator->vertex(point2D[i].x, point2D[i].y, 0.f);
	}

	for (int i = 0; i < 4; i++) {
		Vec2<float> currentPoint = point2D[i];
		Vec2<float> nextPoint = point2D[(i + 1) % 4];
		size_t hash = std::hash<float>()(currentPoint.x) ^
					  std::hash<float>()(currentPoint.y) ^
					  std::hash<float>()(nextPoint.x) ^
					  std::hash<float>()(nextPoint.y);

		lineRenderList[hash] = !lineRenderList[hash];
	}
}

void NewChunks::renderNewChunkOutline(const ChunkPos& chunkPos) {

	float yHeight = 30.f;
	//Too lazy to make option for it

	Vec3<float> point3D[4];
	point3D[0] = Vec3<float>((float)chunkPos.x, yHeight, (float)chunkPos.z);
	point3D[1] = Vec3<float>((float)chunkPos.x + 16.f, yHeight, (float)chunkPos.z);
	point3D[2] = Vec3<float>((float)chunkPos.x + 16.f, yHeight, (float)chunkPos.z + 16.f);
	point3D[3] = Vec3<float>((float)chunkPos.x, yHeight, (float)chunkPos.z + 16.f);

	Vec2<float> point2D[4];
	for (int i = 0; i < 4; i++) {
		if (!MCR::worldToScreen(point3D[i], point2D[i]))
			return;
	}

	for (int i = 0; i < 4; i++) {
		Vec2<float> currentPoint = point2D[i];
		Vec2<float> nextPoint = point2D[(i + 1) % 4];
		size_t hash = std::hash<float>()(currentPoint.x) ^
			std::hash<float>()(currentPoint.y) ^
			std::hash<float>()(nextPoint.x) ^
			std::hash<float>()(nextPoint.y);

		if (lineRenderList[hash]) {
			MCR::tessellator->vertex(currentPoint.x, currentPoint.y, 0.f);
			MCR::tessellator->vertex(nextPoint.x, nextPoint.y, 0.f);
		}
	}
}

void NewChunks::onMCRender(MinecraftUIRenderContext* renderCtx) {
	chunkCount = chunkMap.size();
	if (!Game::canUseMoveKeys())
		return;

	if (chunkMap.empty())
		return;

	LocalPlayer* localPlayer = Game::getLocalPlayer();
	if (localPlayer == nullptr)
		return;

	Vec3<float> lpPos = localPlayer->getPos();
	Vec3<float> lpPos2D = Vec3<float>(lpPos.x, 0.f, lpPos.z);

	for (auto it = chunkMap.begin(); it != chunkMap.end(); ) {
		const ChunkPos& chunkPos = it->second.value();
		float distance = lpPos2D.dist(Vec3<float>(chunkPos.x + 8.5f, 0.f, chunkPos.z + 8.5f));
		if (distance < 256.f) {
			it++;
		}
		else {
			it = chunkMap.erase(it);
		}
	}

	lineRenderList.clear();

	MCR::setColor(mce::Color(1.f, 1.f, 1.f, 1.f));
	MCR::tessellator->begin(VertextFormat::QUAD);
	MCR::tessellator->color(1.f, 0.f, 0.f, 0.35f);
	for (auto& it : chunkMap) {
		const ChunkPos& chunkPos = it.second.value();
		renderNewChunk(chunkPos);
	}
	MeshHelpers::renderMeshImmediately(MCR::screenCtx, MCR::tessellator, MCR::uiMaterial);

	MCR::tessellator->begin(VertextFormat::LINE_LIST);
	MCR::tessellator->color(1.f, 0.f, 0.f, 1.f);
	for (auto& it : chunkMap) {
		const ChunkPos& chunkPos = it.second.value();
		renderNewChunkOutline(chunkPos);
	}
	MeshHelpers::renderMeshImmediately(MCR::screenCtx, MCR::tessellator, MCR::uiMaterial);
}
