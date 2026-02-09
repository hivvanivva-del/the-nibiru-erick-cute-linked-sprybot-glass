#pragma once
#include "../../ModuleBase/Module.h"

class Trident : public Module {
protected:
public:
	Trident();
	virtual void onNormalTick(LocalPlayer* localPlayer) override;
	virtual void onSendPacket(Packet* packet, bool& cancel) override;
};