#pragma once
#include "../../ModuleBase/Module.h"
#include <unordered_map>
#include <string>
#include <chrono>
#include <locale>
#include <codecvt> 
class PacketCriticals : public Module {
private:
public:
    PacketCriticals() : Module("PacketCriticals", "3arthh4ck Chinese Criticals Simplified e", Category::WORLD) {
    }
    void onEnable() override {
        if (!Game::getLocalPlayer() || !Game::getLocalPlayer()->level) return;
        Game::getLocalPlayer()->setStatusFlag(ActorFlags::Critical, true);
    }
    void onDisable() override {
        if (!Game::getLocalPlayer() || !Game::getLocalPlayer()->level) return;
        Game::getLocalPlayer()->setStatusFlag(ActorFlags::Critical, false);
        Game::getLocalPlayer()->setStatusFlag(ActorFlags::Canfly, false);
    }
    void onNormalTick(LocalPlayer* localPlayer) override {
        if (!localPlayer || !localPlayer->level) return;
        localPlayer->setStatusFlag(ActorFlags::Critical, true);
        localPlayer->setStatusFlag(ActorFlags::DelayedAttack, true);
        localPlayer->setStatusFlag(ActorFlags::Canfly, true);
    }
private:
};
