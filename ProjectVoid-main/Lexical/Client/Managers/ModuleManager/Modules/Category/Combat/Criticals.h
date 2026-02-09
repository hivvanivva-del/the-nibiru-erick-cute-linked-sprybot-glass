#pragma once
#include "../../ModuleBase/Module.h"

class Criticals : public Module {
private:
	int mode = 0;
public:
	void sendMovePacket(double height);
public:
	Criticals();
	std::string getModeText() override;
	void onSendPacket(Packet* packet, bool& cancel) override;
	void onAttack(Actor* actor, bool& cancel) override;
};