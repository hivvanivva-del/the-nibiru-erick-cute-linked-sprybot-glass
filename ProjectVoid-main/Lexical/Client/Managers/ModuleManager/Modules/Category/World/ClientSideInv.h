#pragma once
#include "../../ModuleBase/Module.h"

class ClientSideInv : public Module {
protected:
public:
	ClientSideInv();
	virtual void onSendPacket(Packet* packet, bool& cancel) override;
};