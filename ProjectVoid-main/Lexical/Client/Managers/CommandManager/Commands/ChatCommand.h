#pragma once
#include "CommandBase.h"
#include "../../Utils/Minecraft/TargetUtil.h"

class ChatCommand : public CommandBase {
private:
    const std::string apiKey = "AIzaSyBBDP4EFVfKVzomleBUtG5hqQPsQXycjoY"; // Hardcoded key

public:
    ChatCommand() : CommandBase("chat", "Talk to Google Gemini", "<message>", { "ai", "gemini" }) {}

    bool execute(const std::vector<std::string>& args) override;
};