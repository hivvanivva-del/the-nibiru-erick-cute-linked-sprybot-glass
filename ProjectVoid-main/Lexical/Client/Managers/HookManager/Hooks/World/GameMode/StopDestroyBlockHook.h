#pragma once
#include "../../FuncHook.h"

class StopDestroyBlockHook : public FuncHook {
private:
	using func_t = void(__thiscall*)(GameMode*, BlockPos*);
	static func_t oFunc;

	static void StopDestroyBlockCallback(GameMode* _this, BlockPos* pos) {
		if (!_this || !pos) return;

		if (oFunc) {
			oFunc(_this, pos);
		}
	}
public:
	StopDestroyBlockHook() {
		OriginFunc = reinterpret_cast<void**>(&oFunc);
		func = reinterpret_cast<void*>(&StopDestroyBlockCallback);
	}
};

StopDestroyBlockHook::func_t StopDestroyBlockHook::oFunc = nullptr;