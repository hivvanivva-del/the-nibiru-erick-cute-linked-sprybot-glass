#pragma once
#include "../../FuncHook.h"

class ActorSetRotHook : public FuncHook {
private:
	using func_t = float* (__fastcall*)(__int64, __int64, Actor*);
	static func_t oFunc;

	static float* ActorSetRotCallback(__int64 a1, __int64 a2, Actor* a3) {
		if (!a3) return nullptr;

		LocalPlayer* localPlayer = Game::getLocalPlayer();
		if (localPlayer && localPlayer == a3) {
			if (oFunc) {
				float* result = oFunc(a1, a2, a3);
				ModuleManager::onUpdateRotation(localPlayer);
				return result;
			}
			return nullptr;
		}

		if (oFunc) {
			return oFunc(a1, a2, a3);
		}

		return nullptr;
	}

public:
	ActorSetRotHook() {
		OriginFunc = reinterpret_cast<void**>(&oFunc);
		func = reinterpret_cast<void*>(&ActorSetRotCallback);
	}
};

ActorSetRotHook::func_t ActorSetRotHook::oFunc = nullptr;