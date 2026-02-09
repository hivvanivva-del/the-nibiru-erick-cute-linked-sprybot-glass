#pragma once
#include "../../ModuleBase/Module.h"
#include <vector>
#include <string>

class ChatSpammer : public Module {
public:
    enum MessageMode {
        NORMAL = 0,
        PROMOTIONAL = 1,
        TOXIC = 2,
        CUSTOM = 3
    };

    // Settings
    float delay = 5.0f;
    bool randomOrder = false;
    int messageMode = 0;

    // Message lists
    std::vector<std::string> normalMessages;
    std::vector<std::string> promoMessages;
    std::vector<std::string> toxicMessages;
    std::vector<std::string> customMessages;

    // Internal state
    int currentIndex = 0;
    float elapsedTime = 0.f;

    virtual void onNormalTick(LocalPlayer* actor) override;
    void sendNextMessage();
    void addCustomMessage(const std::string& msg);
    void clearCustomMessages();

    ChatSpammer();
};