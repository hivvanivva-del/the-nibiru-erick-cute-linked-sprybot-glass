#pragma once
#include "../FuncHook.h"
#include "../../../../../Renderer/MCR.h"

class RenderLevelHook : public FuncHook {
private:
	using func_t = __int64(__thiscall*)(LevelRenderer*, ScreenContext*, __int64);
	static func_t oFunc;

	static __int64 LevelRenderer_renderLevelCallback(LevelRenderer* _this, ScreenContext* a2, __int64 a3) {
		if (!_this || !a2) {
			return 0;
		}
		__int64 result = 0;
		if (oFunc) {
			result = oFunc(_this, a2, a3);
		}
		else {
			return 0;
		}
		if (Game::getLocalPlayer() != nullptr && _this->levelRendererPlayer != nullptr && MCR::blendMaterial != nullptr) {
			MCR::onRenderWorld(_this, a2);
		}
		return result;
	}

public:
	RenderLevelHook() {
		OriginFunc = reinterpret_cast<void*>(&oFunc);
		func = reinterpret_cast<void*>(&LevelRenderer_renderLevelCallback);
	}
};

RenderLevelHook::func_t RenderLevelHook::oFunc = nullptr;