#pragma once
#include "../../ModuleBase/Module.h"

class Crasher : public Module {
protected:
public:
	Crasher();
	virtual void onNormalTick(LocalPlayer* localPlayer) override;
	virtual void onSendPacket(Packet* packet, bool& cancel) override;
};