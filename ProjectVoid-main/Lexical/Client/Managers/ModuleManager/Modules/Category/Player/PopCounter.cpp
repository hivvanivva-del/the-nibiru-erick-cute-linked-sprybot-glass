#include "PopCounter.h"
#include "../../../../../../Utils/Minecraft/TargetUtil.h"
#include "../../../../../../SDK/Game.h"
#include "../../../../../../SDK/MCTextFormat.h"
#include <string>
#include <memory>
#include "../../../../../../SDK/Network/Packet/TextPacket.h"
#include "../../../../../Client.h"

using namespace std;

constexpr int TOTEM_ID = 584;

PopCounter::PopCounter() : Module("PopCounter", "Counter Pop", Category::PLAYER) {
	// Register settings
	registerSetting(new SliderSetting<int>("Max Count", "Maximum count before reset", &maxCount, 37, 1, 100));
	registerSetting(new SliderSetting<float>("Range", "Range to detect players", &detectionRange, 50.0f, 10.0f, 100.0f));
	//registerSetting(new BoolSetting("Debug Mode", "Show debug information", &debugMode, false));
	registerSetting(new BoolSetting("SendChat", "Announce totem pops in chat", &emuAnnounce, false));
}

static std::vector<Actor*> playerlist;

void PopCounter::onNormalTick(LocalPlayer* actor) {
	playerlist.clear();
	LocalPlayer* player = Game::getLocalPlayer();
	if (player == nullptr) return;

	PlayerInventory* plrInv = player->getplayerInventory();
	if (!plrInv) return;

	Container* inv = plrInv->container;
	GameMode* gm = Game::getLocalPlayer()->getgameMode();
	BlockSource* region = Game::clientInstance->getRegion();
	Level* level = player->getlevel();
	if (!level) return;

	// Get nearby players
	for (Actor* actor : level->getRuntimeActorList()) {
		if (TargetUtil::isTargetValid(actor, false)) {
			float dist = actor->getPos().dist(player->getPos());
			if (dist < this->detectionRange) playerlist.push_back(actor);
		}
	}

	// Debug info: show number of players found
	if (this->debugMode) {
		Game::DisplayClientMessage("[%sDEBUG%s] Found %d players nearby",
			MCTF::YELLOW, MCTF::WHITE, playerlist.size());
	}

	if (!playerlist.empty()) {
		// Reset counter if needed
		if (this->popcounts > this->maxCount) {
			this->popcounts = 0;
		}

		for (Actor* targetPlayer : playerlist) {
			std::string playerName = *targetPlayer->getNameTag();
			ItemStack* offhand = targetPlayer->getOffhandSlot();
			bool hasTotemNow = (offhand && offhand->isValid() && offhand->item.get() && offhand->item.get()->itemId == TOTEM_ID);

			// Only count as pop when the player had a totem last tick, and now does not
			if (playerTotemState[playerName] && !hasTotemNow) {
				this->playerPopCounts[playerName] += 1;
				Client::DisplayClientMessage("%s%s%s Popped a Totem! %s(%d)",
					MCTF::RED, playerName.c_str(),
					MCTF::WHITE, MCTF::STRIKETHROUGH, this->playerPopCounts[playerName]);
				if (this->emuAnnounce) {
					std::string chatMsg = "> @" + playerName + " just popped " + std::to_string(this->playerPopCounts[playerName]) + " totems thanks to Nibiru Client!";
					PlayerUtil::SendTextMessage(chatMsg);
				}
			}
			// Always update the state for next tick
			//SonicWei
			playerTotemState[playerName] = hasTotemNow;
		}
	}
}