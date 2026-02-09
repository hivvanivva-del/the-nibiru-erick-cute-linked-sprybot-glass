#pragma once
#include "../../FuncHook.h"

class ActorSwingHook : public FuncHook {
private:
	using func_t = void(__thiscall*)(Actor*);
	static func_t oFunc;

	static void ActorSwingCallback(Actor* _this) {
		if (!_this) return;

		NoSwing* noSwingMod = ModuleManager::getModule<NoSwing>();
		if (noSwingMod && noSwingMod->isEnabled() && _this == Game::getLocalPlayer()) {
			return;
		}

		if (oFunc) {
			oFunc(_this);
		}
		else {
		}
	}

public:
	ActorSwingHook() {
		OriginFunc = reinterpret_cast<void**>(&oFunc);
		func = reinterpret_cast<void*>(&ActorSwingCallback);
	}
};

ActorSwingHook::func_t ActorSwingHook::oFunc = nullptr;