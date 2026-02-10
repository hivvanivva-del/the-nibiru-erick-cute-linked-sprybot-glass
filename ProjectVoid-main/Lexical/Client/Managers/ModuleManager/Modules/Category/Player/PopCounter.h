#pragma once
#include "../../ModuleBase/Module.h"
#include <unordered_map>
#include <string>

class PopCounter : public Module {
public:
    bool totem = false;
    int popcounts = 0;
    std::string names;

    int maxCount = 37;
    float detectionRange = 50.0f;
    bool debugMode = false;
    bool emuAnnounce = false;
    int announceMode = 0;

    std::unordered_map<std::string, int> playerPopCounts; //runtime data
    std::unordered_map<std::string, bool> playerTotemState;

    PopCounter();

    virtual void onNormalTick(LocalPlayer* actor) override;
};
