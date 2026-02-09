#pragma once
#include "../../FuncHook.h"

class PlayerSlowDownHook : public FuncHook {
private:
	using func_t = void(__thiscall*)(__int64, __int64, __int64);
	static func_t oFunc;

	static void PlayerSlowDownCallback(__int64 a1, __int64 a2, __int64 a3) {
		NoSlowDown* noSlowMod = ModuleManager::getModule<NoSlowDown>();
		if (noSlowMod && noSlowMod->isEnabled()) {
			return;
		}
		if (oFunc) {
			oFunc(a1, a2, a3);
		}
		else {
		}
	}

public:
	PlayerSlowDownHook() {
		OriginFunc = reinterpret_cast<void**>(&oFunc);
		func = reinterpret_cast<void*>(&PlayerSlowDownCallback);
	}
};

PlayerSlowDownHook::func_t PlayerSlowDownHook::oFunc = nullptr;