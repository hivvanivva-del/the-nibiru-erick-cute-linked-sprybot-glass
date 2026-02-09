#pragma once
#include "../../FuncHook.h"

class ActorIsOnFireHook : public FuncHook {
private:
	using func_t = bool(__thiscall*)(Actor*);
	static func_t oFunc;

	static bool ActorIsOnFireCallback(Actor* _this) {
		if (!_this) return false;

		auto localPlayer = Game::getLocalPlayer();
		NoRender* noRenderMod = ModuleManager::getModule<NoRender>();

		if (localPlayer && _this == localPlayer &&
			noRenderMod && noRenderMod->isEnabled() && noRenderMod->noFire) {
			return false;
		}

		if (oFunc) {
			return oFunc(_this);
		}

		return false;
	}

public:
	ActorIsOnFireHook() {
		OriginFunc = reinterpret_cast<void**>(&oFunc);
		func = reinterpret_cast<void*>(&ActorIsOnFireCallback);
	}
};

ActorIsOnFireHook::func_t ActorIsOnFireHook::oFunc = nullptr;