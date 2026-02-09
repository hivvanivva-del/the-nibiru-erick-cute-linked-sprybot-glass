#pragma once
#include "D:\niiibiiru\spammer client nibiru\ProjectVoid-main\ProjectVoid-main\Lexical\SDK\World\Item\ItemStack.h"
#include "D:\niiibiiru\spammer client nibiru\ProjectVoid-main\ProjectVoid-main\Lexical\SDK\Render\ScreenView.h"
//#include "D:\niiibiiru\spammer client nibiru\ProjectVoid-main\ProjectVoid-main\Lexical\StringHash.hpp"
//#include "D:\niiibiiru\spammer client nibiru\ProjectVoid-main\ProjectVoid-main\Lexical\ResourceHelper.hpp"
#include "D:\niiibiiru\spammer client nibiru\ProjectVoid-main\ProjectVoid-main\Lexical\TexturePtr.hpp"
#include "D:\niiibiiru\spammer client nibiru\ProjectVoid-main\ProjectVoid-main\Lexical\SDK\Render\BaseActorRenderContext.h"
#include "D:\niiibiiru\spammer client nibiru\ProjectVoid-main\ProjectVoid-main\Lexical\SDK\Render\ItemRenderer.h"
#include "D:\niiibiiru\spammer client nibiru\ProjectVoid-main\ProjectVoid-main\Lexical\HoverRenderer.hpp"
#include "D:\niiibiiru\spammer client nibiru\ProjectVoid-main\ProjectVoid-main\Lexical\NinesliceInfo.hpp"
#include "D:\niiibiiru\spammer client nibiru\ProjectVoid-main\ProjectVoid-main\Lexical\NinesliceHelper.hpp"

#define SHULKER_CACHE_SIZE 16

class ShulkerRenderer {
private:
	mce::TexturePtr mBackgroundTexture;
	mce::TexturePtr mItemSlotTexture;

public:
	void Render(MinecraftUIRenderContext* ctx, HoverRenderer* hoverRenderer, int index);
};