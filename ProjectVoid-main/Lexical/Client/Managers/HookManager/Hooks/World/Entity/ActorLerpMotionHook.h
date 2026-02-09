#pragma once
#include "../../FuncHook.h"

class ActorLerpMotionHook : public FuncHook {
private:
	using func_t = void(__thiscall*)(Actor*, Vec3<float>*);
	static func_t oFunc;

	static void ActorLerpMotionCallback(Actor* _this, Vec3<float>* delta) {
		if (!_this || !delta) {
			return;
		}

		Velocity* velocityMod = ModuleManager::getModule<Velocity>();
		if (velocityMod && velocityMod->isEnabled() && Game::getLocalPlayer() == _this) {

			static void* networkSender = nullptr;
			if (!networkSender) {
				auto sigAddr = MemoryUtil::findSignature("48 8D 57 38 48 8B CE 48");
				if (sigAddr)
					networkSender = reinterpret_cast<void*>(sigAddr + 0x14);
			}

			if (networkSender && _ReturnAddress() == networkSender) {
				if (_this->stateVector) {
					float hPercent = velocityMod->horizontal / 100.f;
					float vPercent = velocityMod->vertical / 100.f;

					*delta = _this->stateVector->velocity.lerp(*delta, hPercent, vPercent, hPercent);
				}
			}
		}

		if (oFunc) {
			oFunc(_this, delta);
		}
		else {
		}
	}

public:
	ActorLerpMotionHook() {
		OriginFunc = reinterpret_cast<void**>(&oFunc);
		func = reinterpret_cast<void*>(&ActorLerpMotionCallback);
	}
};

ActorLerpMotionHook::func_t ActorLerpMotionHook::oFunc = nullptr;