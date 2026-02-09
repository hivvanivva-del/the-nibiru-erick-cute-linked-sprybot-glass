#pragma once
#include "../../FuncHook.h"

class StartDestroyBlockHook : public FuncHook {
private:
	using func_t = bool(__thiscall*)(GameMode*, BlockPos*, uint8_t, bool*);
	static func_t oFunc;

	static bool StartDestroyBlockCallback(GameMode* _this, BlockPos* pos, uint8_t face, bool* hasDestroyedBlock) {
		if (!_this || !pos || !hasDestroyedBlock) return false;

		if (oFunc) {
			return oFunc(_this, pos, face, hasDestroyedBlock);
		}

		return false;
	}
public:
	StartDestroyBlockHook() {
		OriginFunc = reinterpret_cast<void**>(&oFunc);
		func = reinterpret_cast<void*>(&StartDestroyBlockCallback);
	}
};

StartDestroyBlockHook::func_t StartDestroyBlockHook::oFunc = nullptr;