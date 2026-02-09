#pragma once
#include "../../ModuleBase/Module.h"

class AutoLog : public Module {
public:
	bool uponEnabled = false;
	int mode = 0;
	AutoLog() : Module("AutoLog", "Automatically logs out", Category::MISC) {
		registerSetting(new EnumSetting("Mode", "NULL", { "Kick", "/hub" }, &mode, 0));
		registerSetting(new BoolSetting("On Enable", "Logs automatically when enabled, if this is off then we will only log when u have no totems left", &uponEnabled, false));
	}

	virtual void onNormalTick(LocalPlayer* localPlayer) override {
		int totemCount = 0;
		for (int i = 0; i < 36; i++) {
			ItemStack* item = InventoryUtil::getItem(i);
			if (!InventoryUtil::isValid(item)) continue;
			if (item->item.get()->texture_name == "totem") totemCount++;
		}
		if (totemCount != 0 && !uponEnabled) return;
		if (mode == 0) {
			std::shared_ptr<Packet> sharedPkt = MinecraftPackets::createPacket(PacketID::PlayerAuthInput);
			PlayerAuthInputPacket* authPkt = (PlayerAuthInputPacket*)sharedPkt.get();
			authPkt->position = Vec3<float>(0, 0, 0);
			for (int i = 0; i < 999; i++) {
				localPlayer->level->getPacketSender()->send(authPkt);
			}
		}
		else {
			std::shared_ptr<Packet> sharedPkt = MinecraftPackets::createPacket(PacketID::CommandRequest);
			CommandRequestPacket* cmdPkt = (CommandRequestPacket*)sharedPkt.get();
			cmdPkt->isExternal = false;
			cmdPkt->mCommand = "/hub";
			cmdPkt->origin = 0;
			localPlayer->level->getPacketSender()->send(cmdPkt);
		}
	}
};