#include "AutoHighway.h"
#include "../../../ModuleManager.h"
int blocksPerSecond = 1;
float moveSpeed = 1.f;
int multiPlaceCount = 1;
bool instantMode = false;
AutoHighway::AutoHighway() : Module("HighwayBuilder", "Automatically builds an obsidian highway.", Category::MISC) {
    registerSetting(new EnumSetting("Direction", "Choose highway direction", { "North", "East", "South", "West" }, &direction, 0));
    registerSetting(new SliderSetting<int>("BPS", "Blocks per second", &blocksPerSecond, 1, 1, 100));
    registerSetting(new SliderSetting<int>("MultiPlace", "Blocks placed at once", &multiPlaceCount, 1, 1, 5));
    registerSetting(new SliderSetting<float>("MoveSpeed", "How fast to move the player", &moveSpeed, 0.35f, 0.1f, 0.4f));
    registerSetting(new BoolSetting("InstantMode", "Place all blocks instantly with no delay", &instantMode, false));
}

void AutoHighway::onEnable() {
    tickDelay = 0;
    buildingStage = MINING;
    localPlayer = Game::getLocalPlayer();
    if (!localPlayer) setEnabled(false);
}

void AutoHighway::onNormalTick(LocalPlayer* player) {
    if (!player) return;

    if (!instantMode) {
        if (tickDelay > 0) {
            tickDelay--;
            return;
        }
    }

    player->getMoveInputComponent()->sneaking = true;

    rotatePlayerToDirection(player);

    auto blocksToCheck = getHighwayBlocks(player);

    if (buildingStage == MINING) {
        bool stillMining = false;
        for (const auto& pos : blocksToCheck) {
            auto block = Game::clientInstance->getRegion()->getBlock(pos);
            if (!block || !block->blockLegacy) continue;
            int id = block->blockLegacy->blockId;
            if (id != 0 && id != OBSIDIAN_ID) {
                startMiningBlock(pos);
                stillMining = true;
                break;
            }
        }
        if (!stillMining) {
            buildingStage = PLACING;
        }
    }
    else if (buildingStage == PLACING) {
        if (instantMode) {
            for (const auto& pos : blocksToCheck) {
                auto block = Game::clientInstance->getRegion()->getBlock(pos);
                if (!block || !block->blockLegacy) continue;
                int id = block->blockLegacy->blockId;
                if (id == 0) {
                    placeObsidianAt(pos);
                }
            }
            buildingStage = MOVING;
        }
        else {
            int placed = 0;
            for (const auto& pos : blocksToCheck) {
                if (placed >= multiPlaceCount) break;
                auto block = Game::clientInstance->getRegion()->getBlock(pos);
                if (!block || !block->blockLegacy) continue;
                int id = block->blockLegacy->blockId;
                if (id == 0) {
                    if (placeObsidianAt(pos)) {
                        placed++;
                    }
                }
            }
            if (placed == 0) {
                buildingStage = MOVING;
            }
        }
    }
    else if (buildingStage == MOVING) {
        movePlayerForward(player);
        buildingStage = MINING;
    }

    if (!instantMode) {
        actionDelay = 20 / blocksPerSecond;
        tickDelay = actionDelay;
    }
    else {
        tickDelay = 0;
    }
}

void AutoHighway::rotatePlayerToDirection(LocalPlayer* player) {
    float yaw = 0.f;
    switch (direction) {
    case 0: yaw = 180.f; break;
    case 1: yaw = -90.f; break;
    case 2: yaw = 0.f; break;
    case 3: yaw = 90.f; break;
    }
    player->getActorHeadRotationComponent()->headYaw = yaw;
    player->getMobBodyRotationComponent()->bodyYaw = yaw;
    player->rotation->presentRot.x = 0.f;
}

std::vector<BlockPos> AutoHighway::getHighwayBlocks(LocalPlayer* player) {
    std::vector<BlockPos> blocks;
    AABB localAABB = localPlayer->aabbShape->aabb;
    Vec3<float> pos = localAABB.getCenter();
    pos.y = localAABB.lower.y;
    int dx = 0, dz = 0;
    switch (direction) {
    case 0: dz = -1; break;
    case 1: dx = 1; break;
    case 2: dz = 1; break;
    case 3: dx = -1; break;
    }
    int baseX = (int)pos.x + dx;
    int baseZ = (int)pos.z + dz;
    int floorY = (int)pos.y - 1;
    int railY = floorY + 1;

    for (int offset = -2; offset <= 2; offset++) {
        int worldX = baseX;
        int worldZ = baseZ;
        if (direction == 0 || direction == 2) {
            worldX += offset;
        }
        else {
            worldZ += offset;
        }
        blocks.emplace_back(worldX, floorY, worldZ);
    }

    for (int offset = -2; offset <= 2; offset++) {
        int worldX = baseX;
        int worldZ = baseZ;
        if (direction == 0 || direction == 2) {
            worldX += offset;
        }
        else {
            worldZ += offset;
        }
        if (offset == -2 || offset == 2) {
            blocks.emplace_back(worldX, railY, worldZ);
        }
    }

    int supportX = (int)pos.x;
    int supportZ = (int)pos.z;
    blocks.emplace_back(supportX, floorY, supportZ);

    return blocks;
}

void AutoHighway::startMiningBlock(const BlockPos& pos) {
    PacketMine* packetMine = ModuleManager::getModule<PacketMine>();
    if (!packetMine->isEnabled()) packetMine->setEnabled(true);
    bool b = false;
    localPlayer->gameMode->startDestroyBlock(pos, 0, b);
    packetMine->mineBlock(pos, 0);
}

bool AutoHighway::placeObsidianAt(const BlockPos& pos) {
    int obsidianSlot = findObsidianInHotbar();
    if (obsidianSlot == -1) return false;

    PlayerInventory* inv = localPlayer->getplayerInventory();
    if (inv->selectedSlot != obsidianSlot) {
        inv->selectedSlot = obsidianSlot;
    }

    BlockSource* region = Game::clientInstance->getRegion();
    Block* block = region->getBlock(pos);
    BlockLegacy* blockLegacy = block->blockLegacy;

    if (blockLegacy->canBeBuiltOver(region, pos)) {
        static std::vector<Vec3<int>> checklist = {
            Vec3<int>(0, -1, 0),
            Vec3<int>(0, 1, 0),
            Vec3<int>(0, 0, -1),
            Vec3<int>(0, 0, 1),
            Vec3<int>(-1, 0, 0),
            Vec3<int>(1, 0, 0)
        };

        int face = 0;
        for (const auto& offset : checklist) {
            Vec3<int> neighborPos = pos.sub(offset);
            Block* neighborBlock = region->getBlock(neighborPos);
            if (!neighborBlock->blockLegacy->canBeBuiltOver(region, neighborPos)) {
                return localPlayer->getgameMode()->buildBlock(neighborPos, face, false);
            }
            face++;
        }
    }

    return false;
}

int AutoHighway::findObsidianInHotbar() {
    PlayerInventory* plrInv = localPlayer->getplayerInventory();
    Container* inv = plrInv->container;
    for (int i = 0; i < 9; i++) {
        ItemStack* itemStack = inv->getItem(i);
        if (itemStack->isValid() && itemStack->item->itemId == OBSIDIAN_ID) {
            return i;
        }
    }
    return -1;
}

void AutoHighway::movePlayerForward(LocalPlayer* player) {
    Vec3<float> velocity{ 0.f, 0.f, 0.f };
    float speed = moveSpeed;

    switch (direction) {
    case 0: velocity.z = -speed; break;
    case 1: velocity.x = speed; break;
    case 2: velocity.z = speed; break;
    case 3: velocity.x = -speed; break;
    }

    player->stateVector->velocity = velocity;
}
