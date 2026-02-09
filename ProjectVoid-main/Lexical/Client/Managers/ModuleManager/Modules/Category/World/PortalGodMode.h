#pragma once
#include "../../ModuleBase/Module.h"

class PortalGodMode : public Module {
protected:
public:
	PortalGodMode();
	virtual void onNormalTick(LocalPlayer* localPlayer) override;
	virtual void onSendPacket(Packet* packet, bool& cancel) override;
};