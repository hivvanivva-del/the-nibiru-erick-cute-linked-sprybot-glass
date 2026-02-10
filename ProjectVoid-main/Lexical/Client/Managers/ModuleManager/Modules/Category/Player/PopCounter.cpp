#include "PopCounter.h"
#include "../../../../../../Utils/Minecraft/TargetUtil.h"
#include "../../../../../../SDK/Game.h"
#include "../../../../../../SDK/MCTextFormat.h"
#include <string>
#include <memory>
#include <cstdlib>
#include <ctime>
#include "../../../../../../SDK/Network/Packet/TextPacket.h"
#include "../../../../../Client.h"

using namespace std;

constexpr int TOTEM_ID = 584;

PopCounter::PopCounter() : Module("PopCounter", "Counter Pop", Category::PLAYER) {
    registerSetting(new SliderSetting<int>("Max Count", "Maximum count before reset", &maxCount, 37, 1, 100));
    registerSetting(new SliderSetting<float>("Range", "Range to detect players", &detectionRange, 50.0f, 10.0f, 100.0f));
    registerSetting(new BoolSetting("SendChat", "Announce totem pops in chat", &emuAnnounce, false));

    registerSetting(new EnumSetting("Mode", "Choose announcement type (normal popmessage or randomized messages)", { "Normal", "Randomized" }, &announceMode, 0));

    srand(static_cast<unsigned>(time(nullptr)));
}

static std::vector<Actor*> playerlist;

void PopCounter::onNormalTick(LocalPlayer* actor) {
    playerlist.clear();

    if (!Game::clientInstance) return;

    LocalPlayer* player = Game::getLocalPlayer();
    if (!player) return;

    PlayerInventory* plrInv = player->getplayerInventory();
    if (!plrInv) return;

    Container* inv = plrInv->container;
    GameMode* gm = player->getgameMode();
    BlockSource* region = Game::clientInstance->getRegion();
    Level* level = player->getlevel();

    if (!inv || !gm || !region || !level) return;

    for (Actor* other : level->getRuntimeActorList()) {
        if (!other) continue;
        if (!TargetUtil::isTargetValid(other, false)) continue;

        float dist = other->getPos().dist(player->getPos());
        if (dist < this->detectionRange)
            playerlist.push_back(other);
    }

    if (playerlist.empty()) return;

    if (this->popcounts > this->maxCount)
        this->popcounts = 0;

    static const std::vector<std::string> popMessages = {
        "Don?t run now that I?ve answered your battle cry and popped your %dth totem, @%s!",
        "@%s You are the champion at popping totems! It?s your %dth already",
        "@%s You could make yourself a house from all of those popped totems. %dth totem",
        "@%s Your strategy seems to be hit and run. Must be because its your %dth totem already",
        "Just a bit more pops and youre OUT of totems! %dth already",
        "Your totems have betrayed you. %d totem%s",
        "@%s?s supply of totems seems to be endless. Theyre already at %d",
        "The totems won?t save you forever, @%s, it?s your %dth pop",
        "You should retire at this point @%s, it?s already your %dth totem",
        "@%s?s battle cry has been answered! %dth totem already",
        "@%ss pops are %d and counting!",
        "Ive underestimated the amount of totems @%s has. %dth totem already"
        "Nibiru?s aura has already made @%s?s pop their %dth totem"
        "It seems Nibiru reigns supreme over these AI clients, being @%ss %dth totempop"
        "Pop and RUN! This seems to be the strategy of @%s after their %dth totempop"
        "Your totems weren?t prepared for this, were they @%s? %dth of them have perished already!"
        "@%s?s totems are scared they?ll be next, it?s their %dth pop already"
        "POP, POP!! Your totempops are music to my ears @%s! It?s your %dth and counting!"
        "Bejwyd @%s?if %sghha totemy"
        "@%s?s consumption of totems is making the totem market price go sky high with their %dth totem wasted!"
    };

    for (Actor* targetPlayer : playerlist) {
        if (!targetPlayer) continue;

        std::string playerName;
        if (targetPlayer->getNameTag())
            playerName = *targetPlayer->getNameTag();
        else
            continue;

        ItemStack* offhand = targetPlayer->getOffhandSlot();
        bool hasTotemNow = false;

        if (offhand && offhand->isValid() && offhand->item && offhand->item.get())
            hasTotemNow = (offhand->item.get()->itemId == TOTEM_ID);

        if (playerTotemState[playerName] && !hasTotemNow) {
            this->playerPopCounts[playerName] += 1;
            int pops = this->playerPopCounts[playerName];

            Client::DisplayClientMessage("%s%s%s Popped a Totem! %s(%d)",
                MCTF::RED, playerName.c_str(),
                MCTF::WHITE, MCTF::STRIKETHROUGH, pops);

            if (this->emuAnnounce && Game::clientInstance && Game::getLocalPlayer()) {
                std::string chatMsg;

                if (announceMode == 0) {
                    chatMsg = "> @" + playerName + " just popped " + std::to_string(pops) + " totems thanks to Nibiru Client!";
                }
                else {
                    int index = rand() % popMessages.size();
                    char buffer[200];

                    if (popMessages[index].find("%d totem%s") != std::string::npos) {
                        std::string plural = (pops == 1) ? "" : "s";
                        snprintf(buffer, sizeof(buffer), popMessages[index].c_str(), pops, plural.c_str());
                    }
                    else if (popMessages[index].find("@%s") != std::string::npos && popMessages[index].find("%d") != std::string::npos) {
                        snprintf(buffer, sizeof(buffer), popMessages[index].c_str(), playerName.c_str(), pops);
                    }
                    else if (popMessages[index].find("%d") != std::string::npos) {
                        snprintf(buffer, sizeof(buffer), popMessages[index].c_str(), pops);
                    }
                    else {
                        snprintf(buffer, sizeof(buffer), "%s (%d)", popMessages[index].c_str(), pops);
                    }

                    chatMsg = buffer;
                }

                PlayerUtil::SendTextMessage(chatMsg);
            }
        }

        playerTotemState[playerName] = hasTotemNow;
    }
}
