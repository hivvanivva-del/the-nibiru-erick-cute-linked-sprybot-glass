#pragma once
#include "../FuncHook.h"

class RenderHitSelectHook : public FuncHook {
private:
	using func_t = void(__thiscall*)(LevelRendererPlayer*, BaseActorRenderContext*, BlockSource*, BlockPos*, bool);
	static func_t oFunc;

	static void RenderHitSelectCallback(LevelRendererPlayer* _this, BaseActorRenderContext* renderContext, BlockSource* region, BlockPos* pos, bool fancyGraphics) {
		if (!_this || !renderContext || !region || !pos) {
			return;
		}
		static BlockHighlight* blockHighlightMod = ModuleManager::getModule<BlockHighlight>();
		if (blockHighlightMod && blockHighlightMod->isEnabled()) {
			return;
		}
		if (oFunc) {
			oFunc(_this, renderContext, region, pos, fancyGraphics);
		}
		else {
		}
	}

public:
	RenderHitSelectHook() {
		OriginFunc = reinterpret_cast<void*>(&oFunc);
		func = reinterpret_cast<void*>(&RenderHitSelectCallback);
	}
};

RenderHitSelectHook::func_t RenderHitSelectHook::oFunc = nullptr;