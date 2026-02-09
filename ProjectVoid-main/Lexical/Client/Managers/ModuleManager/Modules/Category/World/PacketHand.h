#pragma once
#include "../../ModuleBase/Module.h"

class PacketHand : public Module {
private:
	int eatingTicks = 0;
public:
	int eatKey = 0x0;
	int eatCooldown = 33;
	bool eatMultiTask = false;
	bool packetPlace = false;
	bool packetAttack = false;
	bool packetEat = false;
public:
	PacketHand() : Module("PacketHand", "Allows u to only packet place/attack", Category::PLAYER) {
		registerSetting(new BoolSetting("Packet Place", "Only places using packets preventing ghost block\nthis may seem like the placement is slow but this is what the other players see even if packet place is off", &packetPlace, false));
		registerSetting(new BoolSetting("Packet Attack", "Attack using packets allowing ur attack to be synced with the ping for a better hit reg", &packetAttack, false));
		registerSetting(new BoolSetting("Packet Eat", "Allows u to silently eat gapples", &packetEat, false));
		registerSetting(new BoolSetting("MultiTask", "Eat while doing stuff", &eatMultiTask, false, [&]() -> bool {
			return packetEat;
		}));
		registerSetting(new KeybindSetting("Eat Key", "Will eat the gapple if the key is pressed, set to NONE to make it auto eat", &eatKey, 0, [&]() -> bool {
			return packetEat;
		}));
		registerSetting(new SliderSetting<int>("Eat Ticks", "Amount of ticks we should eat for, 32 is normal", &eatCooldown, 32, 1, 50, [&]() -> bool {
			return packetEat;
		}));
	}
	virtual void onBuild(const BlockPos& blockPos, const uint8_t face, bool& cancel) override;
	virtual void onAttack(Actor* actor, bool& cancel) override;
	virtual void onNormalTick(LocalPlayer* localPlayer) override;
	virtual void onD2DRender() override;
};