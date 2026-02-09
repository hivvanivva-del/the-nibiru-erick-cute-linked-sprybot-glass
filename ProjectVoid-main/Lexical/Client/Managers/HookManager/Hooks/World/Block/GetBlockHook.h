#pragma once
#include "../../FuncHook.h"

class GetBlockHook : public FuncHook {
private:
	using func_t = Block*(__thiscall*)(BlockSource*, const BlockPos&);
	static inline func_t oFunc;

	static Block* BlockSourceGetBlockCallback(BlockSource* _this, const BlockPos& blockPos) {
		Block* result = oFunc(_this, blockPos);
		static PortalESP* portalEsp = ModuleManager::getModule<PortalESP>();
		static StorageESP* storageEsp = ModuleManager::getModule<StorageESP>();
		static TunnelESP* tunnelEsp = ModuleManager::getModule<TunnelESP>();
		static NewChunks* newChunkMod = ModuleManager::getModule<NewChunks>();
		if (newChunkMod->isEnabled()) {
			static uint16_t checkingBlockIds[3] = { 9, 10, 11 };
			uint16_t currentBlockId = result->blockLegacy->blockId;

			bool hasId = false;
			for (auto& idCheck : checkingBlockIds) {
				if (currentBlockId == idCheck) {
					hasId = true;
					break;
				}
			}

			if (hasId) {
				if (oFunc(_this, BlockPos(blockPos.x, blockPos.y - 1, blockPos.z))->blockLegacy->blockId == 0) {
					ChunkPos chunkPos = ChunkPos(blockPos);
					uint64_t chunkHash = std::hash<int>()(chunkPos.x) ^ std::hash<int>()(chunkPos.z);
					if (!newChunkMod->chunkMap[chunkHash].has_value()) {
						newChunkMod->chunkMap[chunkHash] = chunkPos;
						NotificationManager::addNotification("New chunk detected!", 3.f);
					}
				}
			}
		}
		if (storageEsp->isEnabled()) {
			int blockId = oFunc(_this, blockPos)->blockLegacy->blockId;
			if (blockId == 54) { // chest
				storageEsp->blockList[blockPos] = UIColor(255, 165, 0);
			}
			else if (blockId == 130) { // ender chest
				storageEsp->blockList[blockPos] = UIColor(255, 0, 255);
			}
			else if (blockId == 205 || blockId == 218) { // shulker
				storageEsp->blockList[blockPos] = UIColor(255, 255, 255);
			}
			else if (blockId == 458) { // barrel
				storageEsp->blockList[blockPos] = UIColor(155, 75, 0);
			}
			else if (blockId == 154) { // hopper
				storageEsp->blockList[blockPos] = UIColor(128, 128, 128);
			}
			else if (blockId == 61 || blockId == 451 || blockId == 453) { // furnace
				storageEsp->blockList[blockPos] = UIColor(70, 70, 70);
			}
		}
		if (portalEsp->isEnabled()) {
			if (oFunc(_this, blockPos)->blockLegacy->blockId == 90) {
				portalEsp->portalList.insert(blockPos);
			}
		}
		if (tunnelEsp->isEnabled()) {
			auto isTunnel = [&]() {
				static std::vector<BlockPos> xTunnel = { {1, 0, 0}, {-1, 0, 0}, {1, 1, 0}, {-1, 1, 0} };
				static std::vector<BlockPos> zTunnel = { {1, 0, 0}, {-1, 0, 0}, {1, 1, 0}, {-1, 1, 0} };

				if (!oFunc(_this, blockPos.add(0, -1, 0))->blockLegacy->isSolid()) {
					return false;
				}
				if (!oFunc(_this, blockPos.add(0, 2, 0))->blockLegacy->isSolid()) {
					return false;
				}

				if (oFunc(_this, blockPos)->blockLegacy->isSolid()) {
					return false;
				}
				if (oFunc(_this, blockPos.add(0, 1, 0))->blockLegacy->isSolid()) {
					return false;
				}

				int check = 0;
				for (BlockPos tun : xTunnel) {
					BlockPos pos = blockPos.add(tun);
					if (!oFunc(_this, pos)->blockLegacy->isSolid()) {
						check++;
						break;
					}
				}
				for (BlockPos tun : zTunnel) {
					BlockPos pos = blockPos.add(tun);
					if (!oFunc(_this, pos)->blockLegacy->isSolid()) {
						check++;
						break;
					}
				}

				if (check != 2) {
					return true;
				}

				return false;
			};
			if (isTunnel()) {
				tunnelEsp->tunnelList.insert(blockPos);
			}
		}
		return result;
	}
public:
	GetBlockHook() {
		OriginFunc = (void*)&oFunc;
		func = (void*)&BlockSourceGetBlockCallback;
	}
};