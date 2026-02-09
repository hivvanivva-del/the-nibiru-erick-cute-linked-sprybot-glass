#pragma once
#include "../../FuncHook.h"

class MobSwingDurationHook : public FuncHook {
private:
	using func_t = uint32_t(__thiscall*)(Mob*);
	static func_t oFunc;

	static uint32_t MobGetCurrentSwingDurationCallback(Mob* _this) {
		if (!_this) return 0;

		Swing* swingMod = ModuleManager::getModule<Swing>();
		if (swingMod && swingMod->isEnabled() && Game::getLocalPlayer() == _this) {
			return swingMod->swingSpeed;
		}

		if (oFunc) {
			return oFunc(_this);
		}
		else {
			return 0;
		}
	}

public:
	MobSwingDurationHook() {
		OriginFunc = reinterpret_cast<void**>(&oFunc);
		func = reinterpret_cast<void*>(&MobGetCurrentSwingDurationCallback);
	}
};

MobSwingDurationHook::func_t MobSwingDurationHook::oFunc = nullptr;