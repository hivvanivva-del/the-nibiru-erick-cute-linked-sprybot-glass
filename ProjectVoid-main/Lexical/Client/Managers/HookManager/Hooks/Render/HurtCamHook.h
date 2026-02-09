#pragma once
#include "../FuncHook.h"

class HurtCamHook : public FuncHook {
private:
	using func_t = __int64(__thiscall*)(Actor*, char, uint32_t);
	static inline func_t oFunc;

	static __int64 CauseHurtCamCallback(Actor* a1, char a2, uint32_t a3) {
		__int64 result = oFunc(a1, a2, a3);

		static NoHurtCam* noHurtCamMod = ModuleManager::getModule<NoHurtCam>();

		if (noHurtCamMod && noHurtCamMod->isEnabled() && a2 == 2 && a1 == Game::getLocalPlayer()) {
			uint32_t* hurtTime = *(uint32_t**)((uintptr_t)a1 + 0x5A8);
			if (hurtTime) {
				*hurtTime = 0;
			}
		}
		return result;
	}
public:
	HurtCamHook() {
		OriginFunc = (void*)&oFunc;
		func = (void*)&CauseHurtCamCallback;
	}
};