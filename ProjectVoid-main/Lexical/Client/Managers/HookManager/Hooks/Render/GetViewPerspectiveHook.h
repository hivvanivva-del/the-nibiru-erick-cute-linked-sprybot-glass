#pragma once
#include "../FuncHook.h"

class GetViewPerspectiveHook : public FuncHook {
private:
	using func_t = int(__thiscall*)(__int64);
	static inline func_t oFunc;

	static int GetViewPerspectiveCallback(__int64 a1) {
		int result = oFunc(a1);
		Game::viewPerspectiveMode = result;
		return result;
	}

public:
	GetViewPerspectiveHook() {
		OriginFunc = reinterpret_cast<void*>(&oFunc);
		func = reinterpret_cast<void*>(&GetViewPerspectiveCallback);
	}
};