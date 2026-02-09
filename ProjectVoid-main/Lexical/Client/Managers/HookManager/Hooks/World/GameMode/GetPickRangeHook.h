#pragma once
#include "../../FuncHook.h"

class GetPickRangeHook : public FuncHook {
private:
	using func_t = float(__thiscall*)(GameMode*, const InputMode&, bool);
	static func_t oFunc;

	static float GameModeGetPickRangeCallback(GameMode* _this, const InputMode& currentInputMode, bool isVR) {
		if (!_this) return 4.0f;

		BlockReach* blockReachMod = ModuleManager::getModule<BlockReach>();
		if (blockReachMod && blockReachMod->isEnabled()) {
			return blockReachMod->blockReach;
		}

		if (oFunc) {
			return oFunc(_this, currentInputMode, isVR);
		}
		return 4.0f;
	}

public:
	GetPickRangeHook() {
		OriginFunc = reinterpret_cast<void**>(&oFunc);
		func = reinterpret_cast<void*>(&GameModeGetPickRangeCallback);
	}
};

GetPickRangeHook::func_t GetPickRangeHook::oFunc = nullptr;