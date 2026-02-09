#pragma once
#include <d3d11.h>
#include <d3d12.h>
#include <d3d11on12.h>
#include <d2d1_3.h>
#include <dxgi1_4.h>
#include <dwrite_3.h>

#include <string>

#include "../Utils/Maths.h"
#include "../Utils/ColorUtil.h"

namespace D2D {
	extern float deltaTime;

	void NewFrame(IDXGISwapChain3* swapChain, ID3D11Device* d3d11Device, float fxdpi);
	void EndFrame();
	void Render();
	void Clean();
	void Flush();
	void InitWICFactory();
	void CleanupWICFactory();
	Vec2<float> getWindowSize();
	void drawText(const Vec2<float>& textPos, const std::string& textStr, const UIColor& color, float textSize = 1.f, bool storeTextLayout = true);
	float getTextWidth(const std::string& textStr, float textSize = 1.f, bool storeTextLayout = true);
	float getTextHeight(const std::string& textStr, float textSize = 1.f, bool storeTextLayout = true);
	void drawLine(const Vec2<float>& startPos, const Vec2<float>& endPos, const UIColor& color, float width = 1.f);
	void drawRectangle(const Vec4<float>& rect, const UIColor& color, float width = 1.f);
	void fillRectangle(const Vec4<float>& rect, const UIColor& color);
	void drawCircle(const Vec2<float>& centerPos, const UIColor& color, float radius, float width = 1.f);
	void fillCircle(const Vec2<float>& centerPos, const UIColor& color, float radius);
	void addBlur(const Vec4<float>& rect, float strength, bool flush = true);
	void drawGlowingRectangle(const Vec4<float>& rect, const UIColor& color, float glowRadius, float cornerRadius = 0.0f);
	void drawParticleNetwork(const Vec2<float>& screenSize, const UIColor& color, int particleCount, float lineWidth, float maxDistance);
	void drawRainParticles(const Vec2<float>& screenSize, const UIColor& color, int particleCount, float speed, float length);
	void drawImageAt3DPos(const Vec3<float>& worldPos, ID2D1Bitmap* bitmap, float size);
	void drawNamePng(float x, float y, float width, float height);
	ID2D1Bitmap* loadBitmapFromFile(const std::wstring& filePath);
	void drawGlowingText(const Vec2<float>& textPos, const std::string& textStr, const UIColor& color, float textSize, float glowSize);
	void drawStarParticles(const Vec2<float>& screenSize, const UIColor& color, int particleCount, float speed, float size);
	void fillRoundedRectangle(const Vec4<float>& rect, const UIColor& color, float radius);
	void drawTriangle(const Vec2<float>& p1, const Vec2<float>& p2, const Vec2<float>& p3, const UIColor& color, float width = 1.f);
	void fillTriangle(const Vec2<float>& p1, const Vec2<float>& p2, const Vec2<float>& p3, const UIColor& color);
};
