#pragma once
#include "../../ModuleBase/Module.h"

class NewChunks : public Module {
public:
	std::unordered_map<uint64_t, std::optional<ChunkPos>> chunkMap;
	size_t chunkCount = 0;
private:
	std::unordered_map<size_t, bool> lineRenderList;
	void renderNewChunk(const ChunkPos& chunkPos);
	void renderNewChunkOutline(const ChunkPos& chunkPos);
public:
	NewChunks();

	std::string getModeText() override;
	void onMCRender(MinecraftUIRenderContext* renderCtx) override;
};