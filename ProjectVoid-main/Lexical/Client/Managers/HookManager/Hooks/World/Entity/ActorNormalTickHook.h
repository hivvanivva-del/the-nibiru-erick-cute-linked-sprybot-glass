#pragma once
#include "../../FuncHook.h"

class ActorNormalTickHook : public FuncHook {
private:
	using func_t = void(__thiscall*)(Actor*);
	static func_t oFunc;

	static void ActorNormalTickCallback(Actor* _this) {
		if (!_this) return;

		LocalPlayer* localPlayer = Game::getLocalPlayer();
		if (localPlayer && localPlayer == _this) {
			ModuleManager::onNormalTick(localPlayer);
		}

		if (oFunc) {
			oFunc(_this);
		}
		else {
		}
	}

public:
	ActorNormalTickHook() {
		OriginFunc = reinterpret_cast<void**>(&oFunc);
		func = reinterpret_cast<void*>(&ActorNormalTickCallback);
	}
};

ActorNormalTickHook::func_t ActorNormalTickHook::oFunc = nullptr;