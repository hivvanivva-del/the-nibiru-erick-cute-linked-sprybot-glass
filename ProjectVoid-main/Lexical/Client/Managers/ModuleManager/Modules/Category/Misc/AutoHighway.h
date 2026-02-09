#pragma once

#include "../../ModuleBase/Module.h"
#include <vector>

class LocalPlayer;
class PacketMine;

enum Stage {
    MINING,
    PLACING,
    MOVING
};

class AutoHighway : public Module {
public:
    AutoHighway();

    void onEnable() override;
    void onNormalTick(LocalPlayer* player) override;

private:
    void rotatePlayerToDirection(LocalPlayer* player);
    std::vector<BlockPos> getHighwayBlocks(LocalPlayer* player);
    void startMiningBlock(const BlockPos& pos);
    bool placeObsidianAt(const BlockPos& pos);
    int findObsidianInHotbar();
    void movePlayerForward(LocalPlayer* player);

    LocalPlayer* localPlayer = nullptr;
    Stage buildingStage = MINING;
    int tickDelay = 0;

    // enum direction as int (0-North, 1-East, 2-South, 3-West)
    int direction = 0;
    int actionDelay = 2;

    static constexpr int OBSIDIAN_ID = 49;
};
