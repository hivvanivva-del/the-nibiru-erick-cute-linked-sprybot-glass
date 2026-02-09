#pragma once
#include "../../ModuleBase/Module.h"
#include <unordered_map>

class PopCounter : public Module {
public:
    bool totem = false;
    int popcounts = 0;
    std::string names;

    // Settings
    int maxCount = 37;  // Maximum count before reset
    float detectionRange = 50.0f;  // Detection range for players
    bool debugMode = false;  // Debug mode for testing
    bool emuAnnounce = false;

    std::unordered_map<std::string, int> playerPopCounts;
    std::unordered_map<std::string, bool> playerTotemState;

    virtual void onNormalTick(LocalPlayer* actor) override;
    PopCounter();
};