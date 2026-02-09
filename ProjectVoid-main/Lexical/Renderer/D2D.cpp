#include "D2D.h"
#include <unordered_map>
#include <winrt/base.h>
#include "../Client/Client.h"
#include "../SDK/Game.h"
#include "../Utils/TimerUtil.h"
#include "../Utils/Logger.h"

float D2D::deltaTime = 0.016f;

//d2d stuff
static ID2D1Factory3* d2dFactory = nullptr;
static IDWriteFactory* d2dWriteFactory = nullptr;
static ID2D1Device2* d2dDevice = nullptr;
static ID2D1DeviceContext2* d2dDeviceContext = nullptr;
static ID2D1Bitmap1* sourceBitmap = nullptr;
static ID2D1Effect* blurEffect = nullptr;

//cache
static std::unordered_map<float, winrt::com_ptr<IDWriteTextFormat>> textFormatCache;
static std::unordered_map<uint64_t, winrt::com_ptr<IDWriteTextLayout>> textLayoutCache;
static std::unordered_map<uint32_t, winrt::com_ptr<ID2D1SolidColorBrush>> colorBrushCache;

//temporary cache
static std::unordered_map<uint64_t, winrt::com_ptr<IDWriteTextLayout>> textLayoutTemporary;

static int currentD2DFontSize = 25;
static std::string currentD2DFont = "Arial";
static bool isFontItalic = false;

static bool initD2D = false;

template<typename T>
void SafeRelease(T*& ptr) {
	if (ptr != nullptr) {
		ptr->Release();
		ptr = nullptr;
	}
}

std::wstring to_wide(const std::string& str);
uint64_t getTextLayoutKey(const std::string& textStr, float textSize);
IDWriteTextFormat* getTextFormat(float textSize);
IDWriteTextLayout* getTextLayout(const std::string& textStr, float textSize, bool storeTextLayout = true);
ID2D1SolidColorBrush* getSolidColorBrush(const UIColor& color);

void D2D::NewFrame(IDXGISwapChain3* swapChain, ID3D11Device* d3d11Device, float fxdpi) {
	if (!initD2D) {

		D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, &d2dFactory);

		DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(d2dWriteFactory), reinterpret_cast<IUnknown**>(&d2dWriteFactory));

		IDXGIDevice* dxgiDevice;
		d3d11Device->QueryInterface<IDXGIDevice>(&dxgiDevice);
		d2dFactory->CreateDevice(dxgiDevice, &d2dDevice);
		dxgiDevice->Release();

		d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &d2dDeviceContext);
		//d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_ENABLE_MULTITHREADED_OPTIMIZATIONS, &d2dDeviceContext);

		d2dDeviceContext->CreateEffect(CLSID_D2D1GaussianBlur, &blurEffect);
		blurEffect->SetValue(D2D1_GAUSSIANBLUR_PROP_BORDER_MODE, D2D1_BORDER_MODE_HARD);
		blurEffect->SetValue(D2D1_GAUSSIANBLUR_PROP_OPTIMIZATION, D2D1_GAUSSIANBLUR_OPTIMIZATION_QUALITY);

		IDXGISurface* dxgiBackBuffer = nullptr;
		swapChain->GetBuffer(0, IID_PPV_ARGS(&dxgiBackBuffer));
		D2D1_BITMAP_PROPERTIES1 bitmapProperties
			= D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
				D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED), fxdpi, fxdpi);
		d2dDeviceContext->CreateBitmapFromDxgiSurface(dxgiBackBuffer, &bitmapProperties, &sourceBitmap);
		dxgiBackBuffer->Release();

		d2dDeviceContext->SetTarget(sourceBitmap);

		initD2D = true;
	}

	d2dDeviceContext->BeginDraw();
}

void D2D::EndFrame() {
	if (!initD2D)
		return;

	d2dDeviceContext->EndDraw();

	static CustomFont* customFontMod = ModuleManager::getModule<CustomFont>();
	if ((currentD2DFont != customFontMod->getSelectedFont()) || (currentD2DFontSize != customFontMod->fontSize) || (isFontItalic != customFontMod->italic)) {
		currentD2DFont = customFontMod->getSelectedFont();
		currentD2DFontSize = customFontMod->fontSize;
		isFontItalic = customFontMod->italic;
		textFormatCache.clear();
		textLayoutCache.clear();
		//textLayoutTemporary.clear();
	}

	static float timeCounter = 0.0f;
	timeCounter += D2D::deltaTime;
	if (timeCounter > 90.f) {
		if (textFormatCache.size() > 1000)
			textFormatCache.clear();

		if (textLayoutCache.size() > 500)
			textLayoutCache.clear();

		if (colorBrushCache.size() > 2000)
			colorBrushCache.clear();

		timeCounter = 0.0f;
	}

	textLayoutTemporary.clear();
}

void D2D::Render() {

	// NameTags
	{
		// Prevents NamTags render override HUD and Watermark
		static NameTags* nameTagsMod = ModuleManager::getModule<NameTags>();
		if (nameTagsMod->isEnabled())
			nameTagsMod->RenderD2D();
	}

	ModuleManager::onD2DRender();
	NotificationManager::Render();

	/*
	// Debug
	{
		char textFormatDb[30];
		sprintf_s(textFormatDb, 30, "TextFormatCaches: %i", (int)(textFormatCache.size()));

		char textLayoutDb[30];
		sprintf_s(textLayoutDb, 30, "TextLayoutCaches: %i", (int)(textLayoutCache.size()));

		char colorBrushDb[30];
		sprintf_s(colorBrushDb, 30, "ColorBrushCaches: %i", (int)(colorBrushCache.size()));

		drawText(Vec2<float>(10.f, 75.f), std::string(textFormatDb), UIColor(255, 255, 255), 0.5f, false);
		drawText(Vec2<float>(10.f, 90.f), std::string(textLayoutDb), UIColor(255, 255, 255), 0.5f, false);
		drawText(Vec2<float>(10.f, 105.f), std::string(colorBrushDb), UIColor(255, 255, 255), 0.5f, false);
	}
	*/
	{   // Yuh
		static Colors* colorMod = ModuleManager::getModule<Colors>();
		for (Module* mod : ModuleManager::moduleList) {
			for (Setting* setting : mod->getSettingList()) {
				if (setting->type != SettingType::COLOR_S) continue;
				ColorSetting* colorSetting = static_cast<ColorSetting*>(setting);
				if (colorSetting->colorSynced && colorSetting->showSynced) {
					colorSetting->colorPtr->r = colorMod->getColor().r;
					colorSetting->colorPtr->g = colorMod->getColor().g;
					colorSetting->colorPtr->b = colorMod->getColor().b;
				}
			}
		}
	}
	// ClickGUI
	{
		static ClickGUI* clickGuiMod = ModuleManager::getModule<ClickGUI>();
		if (clickGuiMod->isEnabled()) clickGuiMod->Render();
	}
	// Eject
	{
		Vec2<float> windowSize = D2D::getWindowSize();
		static float holdTime = 0.f;
		static float holdAnim = 0.f;
		static float showDuration = 0.f;
		static float exitDuration = 0.f;
		static float exitVelocity = 0.f;

		if (showDuration > 0.1f) {
			static std::string text = "Hold Ctrl and L to eject";
			float textSize = 1.f * showDuration;
			float textPaddingX = 4.f * textSize;
			float textPaddingY = 1.f * textSize;
			float textWidth = getTextWidth(text, textSize);
			float textHeight = getTextHeight(text, textSize);

			Vec2<float> textPos = Vec2<float>((windowSize.x - textWidth) / 2.f, -30.f + (100.f * showDuration));
			Vec4<float> rectPos = Vec4<float>(textPos.x - textPaddingX, textPos.y - textPaddingY, textPos.x + textWidth + textPaddingX, textPos.y + textHeight + textPaddingY);
			Vec4<float> underlineRect = Vec4<float>(rectPos.x, rectPos.w, rectPos.z, rectPos.w + 2.f * textSize);
			Vec4<float> underlineAnim = Vec4<float>(underlineRect.x, underlineRect.y, underlineRect.x + (underlineRect.z - underlineRect.x) * holdAnim, underlineRect.w);

			fillRectangle(rectPos, UIColor(0, 0, 0, 125));
			fillRectangle(underlineRect, UIColor(0, 0, 0, 165));
			fillRectangle(underlineAnim, UIColor(255, 255, 255));
			drawText(textPos, text, UIColor(255, 255, 255), textSize);
		}

		if (Game::isKeyDown(VK_CONTROL) && Game::isKeyDown('L')) {
			holdTime += D2D::deltaTime / 2.f;
			if (holdTime > 1.f)
				holdTime = 1.f;
			exitDuration = 1.5f;
		}
		else {
			holdTime = 0.f;
			exitDuration -= D2D::deltaTime;
		}

		holdAnim += (holdTime - holdAnim) * (D2D::deltaTime * 10.f);
		if (holdAnim > 1.f)
			holdAnim = 1.f;
		if (holdAnim < 0.f)
			holdAnim = 0.f;

		if (exitDuration > 0.f) {
			showDuration += (1.f - showDuration) * (D2D::deltaTime * 8.f);
			exitVelocity = 0.f;
		}
		else {
			showDuration -= exitVelocity;
			exitVelocity += D2D::deltaTime / 4.f;
		}

		if (showDuration < 0.f)
			showDuration = 0.f;
		if (showDuration > 1.f)
			showDuration = 1.f;

		if (holdAnim > 0.99f)
			Client::shutdown();
	}
	
}

void D2D::Clean() {
	if (!initD2D)
		return;

	SafeRelease(d2dFactory);
	SafeRelease(d2dWriteFactory);
	SafeRelease(d2dDevice);
	SafeRelease(d2dDeviceContext);
	SafeRelease(sourceBitmap);
	SafeRelease(blurEffect);

	textFormatCache.clear();
	textLayoutCache.clear();
	colorBrushCache.clear();

	textLayoutTemporary.clear();

	initD2D = false;
}

void D2D::Flush() {
	d2dDeviceContext->Flush();
}

Vec2<float> D2D::getWindowSize() {
	D2D1_SIZE_U size = sourceBitmap->GetPixelSize();
	return Vec2<float>((float)size.width, (float)size.height);
}

void D2D::drawText(const Vec2<float>& textPos, const std::string& textStr, const UIColor& color, float textSize, bool storeTextLayout) {
	IDWriteTextLayout* textLayout = getTextLayout(textStr, textSize, storeTextLayout);

	static CustomFont* customFontMod = ModuleManager::getModule<CustomFont>();
	if (customFontMod->shadow) {
		ID2D1SolidColorBrush* shadowColorBrush = getSolidColorBrush(UIColor(0, 0, 0, color.a));
		d2dDeviceContext->DrawTextLayout(D2D1::Point2F(textPos.x + 1.f, textPos.y + 1.f), textLayout, shadowColorBrush);
	}

	ID2D1SolidColorBrush* colorBrush = getSolidColorBrush(color);
	d2dDeviceContext->DrawTextLayout(D2D1::Point2F(textPos.x, textPos.y), textLayout, colorBrush);
}

float D2D::getTextWidth(const std::string& textStr, float textSize, bool storeTextLayout) {
	IDWriteTextLayout* textLayout = getTextLayout(textStr, textSize, storeTextLayout);
	DWRITE_TEXT_METRICS textMetrics;
	textLayout->GetMetrics(&textMetrics);

	return textMetrics.widthIncludingTrailingWhitespace;
}

float D2D::getTextHeight(const std::string& textStr, float textSize, bool storeTextLayout) {
	IDWriteTextLayout* textLayout = getTextLayout(textStr, textSize, storeTextLayout);
	DWRITE_TEXT_METRICS textMetrics;
	textLayout->GetMetrics(&textMetrics);

	return std::ceilf(textMetrics.height);
}

void D2D::drawLine(const Vec2<float>& startPos, const Vec2<float>& endPos, const UIColor& color, float width) {
	ID2D1SolidColorBrush* colorBrush = getSolidColorBrush(color);
	d2dDeviceContext->DrawLine(D2D1::Point2F(startPos.x, startPos.y), D2D1::Point2F(endPos.x, endPos.y), colorBrush, width);
}

void D2D::drawRectangle(const Vec4<float>& rect, const UIColor& color, float width) {
	ID2D1SolidColorBrush* colorBrush = getSolidColorBrush(color);
	d2dDeviceContext->DrawRectangle(D2D1::RectF(rect.x, rect.y, rect.z, rect.w), colorBrush, width);
}

void D2D::fillRectangle(const Vec4<float>& rect, const UIColor& color) {
	ID2D1SolidColorBrush* colorBrush = getSolidColorBrush(color);
	d2dDeviceContext->FillRectangle(D2D1::RectF(rect.x, rect.y, rect.z, rect.w), colorBrush);
}

void D2D::drawCircle(const Vec2<float>& centerPos, const UIColor& color, float radius, float width) {
	ID2D1SolidColorBrush* colorBrush = getSolidColorBrush(color);
	d2dDeviceContext->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(centerPos.x, centerPos.y), radius, radius), colorBrush, width);
}

void D2D::fillCircle(const Vec2<float>& centerPos, const UIColor& color, float radius) {
	ID2D1SolidColorBrush* colorBrush = getSolidColorBrush(color);
	d2dDeviceContext->FillEllipse(D2D1::Ellipse(D2D1::Point2F(centerPos.x, centerPos.y), radius, radius), colorBrush);
}

void D2D::drawGlowingRectangle(const Vec4<float>& rect, const UIColor& color, float glowRadius, float glowCornerRadius /* = 0.0f */) {
	// First create the base rectangle (always sharp corners)
	ID2D1SolidColorBrush* solidBrush = getSolidColorBrush(color);
	d2dDeviceContext->FillRectangle(D2D1::RectF(rect.x, rect.y, rect.z, rect.w), solidBrush);

	// Create a larger rectangle for the glow
	D2D1_RECT_F glowRect = D2D1::RectF(
		rect.x - glowRadius,
		rect.y - glowRadius,
		rect.z + glowRadius,
		rect.w + glowRadius
	);

	// Create a compatible render target for blur effect
	winrt::com_ptr<ID2D1BitmapRenderTarget> blurTarget;
	d2dDeviceContext->CreateCompatibleRenderTarget(
		D2D1::SizeF(glowRect.right - glowRect.left, glowRect.bottom - glowRect.top),
		blurTarget.put()
	);

	// Draw the glow source with specified corner radius
	blurTarget->BeginDraw();
	blurTarget->Clear(D2D1::ColorF(0, 0, 0, 0));

	ID2D1SolidColorBrush* glowBrush;
	blurTarget->CreateSolidColorBrush(
		D2D1::ColorF(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, 1.0f),
		&glowBrush
	);

	D2D1_RECT_F innerGlowRect = D2D1::RectF(
		glowRadius,
		glowRadius,
		(rect.z - rect.x) + glowRadius,
		(rect.w - rect.y) + glowRadius
	);

	if (glowCornerRadius > 0.0f) {
		D2D1_ROUNDED_RECT roundedGlow = D2D1::RoundedRect(
			innerGlowRect,
			glowCornerRadius,
			glowCornerRadius
		);
		blurTarget->FillRoundedRectangle(roundedGlow, glowBrush);
	}
	else {
		blurTarget->FillRectangle(innerGlowRect, glowBrush);
	}
	blurTarget->EndDraw();

	// Get the bitmap and apply blur
	winrt::com_ptr<ID2D1Bitmap> glowBitmap;
	blurTarget->GetBitmap(glowBitmap.put());

	winrt::com_ptr<ID2D1Effect> blurEffect;
	d2dDeviceContext->CreateEffect(CLSID_D2D1GaussianBlur, blurEffect.put());
	blurEffect->SetInput(0, glowBitmap.get());
	blurEffect->SetValue(D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION, glowRadius / 2.0f);
	blurEffect->SetValue(D2D1_GAUSSIANBLUR_PROP_BORDER_MODE, D2D1_BORDER_MODE_HARD);

	// Draw the blurred glow
	d2dDeviceContext->DrawImage(
		blurEffect.get(),
		D2D1::Point2F(glowRect.left, glowRect.top),
		D2D1::RectF(0, 0, glowRect.right - glowRect.left, glowRect.bottom - glowRect.top)
	);

	// Redraw the original rectangle (sharp corners) on top
	d2dDeviceContext->FillRectangle(D2D1::RectF(rect.x, rect.y, rect.z, rect.w), solidBrush);

	// Cleanup
	glowBrush->Release();
}

void D2D::addBlur(const Vec4<float>& rect, float strength, bool flush) {
	if (flush) {
		d2dDeviceContext->Flush();
	}
	ID2D1Bitmap* targetBitmap = nullptr;
	D2D1_BITMAP_PROPERTIES props = D2D1::BitmapProperties(sourceBitmap->GetPixelFormat());
	d2dDeviceContext->CreateBitmap(sourceBitmap->GetPixelSize(), props, &targetBitmap);
	D2D1_POINT_2U destPoint = D2D1::Point2U(0, 0);
	D2D1_SIZE_U size = sourceBitmap->GetPixelSize();
	D2D1_RECT_U Rect = D2D1::RectU(0, 0, size.width, size.height);
	targetBitmap->CopyFromBitmap(&destPoint, sourceBitmap, &Rect);

	D2D1_RECT_F screenRectF = D2D1::RectF(0.f, 0.f, (float)sourceBitmap->GetPixelSize().width, (float)sourceBitmap->GetPixelSize().height);
	D2D1_RECT_F clipRectD2D = D2D1::RectF(rect.x, rect.y, rect.z, rect.w);

	blurEffect->SetInput(0, targetBitmap);
	blurEffect->SetValue(D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION, strength);

	ID2D1Image* outImage = nullptr;
	blurEffect->GetOutput(&outImage);

	ID2D1ImageBrush* outImageBrush = nullptr;
	D2D1_IMAGE_BRUSH_PROPERTIES outImage_props = D2D1::ImageBrushProperties(screenRectF);
	d2dDeviceContext->CreateImageBrush(
		outImage,
		outImage_props,
		&outImageBrush
	);

	ID2D1RectangleGeometry* clipRectGeo = nullptr;
	d2dFactory->CreateRectangleGeometry(clipRectD2D, &clipRectGeo);
	d2dDeviceContext->FillGeometry(clipRectGeo, outImageBrush);

	targetBitmap->Release();
	outImage->Release();
	outImageBrush->Release();
	clipRectGeo->Release();
}

void D2D::drawParticleNetwork(const Vec2<float>& screenSize, const UIColor& color, int particleCount, float lineWidth, float maxDistance) {
	// Create particles if they don't exist
	static std::vector<Vec2<float>> particles;
	static std::vector<Vec2<float>> velocities;
	static std::vector<Vec2<float>> targets;
	static bool initialized = false;

	if (!initialized) {
		particles.resize(particleCount);
		velocities.resize(particleCount);
		targets.resize(particleCount);

		// Initialize particles randomly across the screen
		for (int i = 0; i < particleCount; i++) {
			particles[i].x = (float)(rand() % (int)screenSize.x);
			particles[i].y = (float)(rand() % (int)screenSize.y);

			// Extremely slow initial velocity
			velocities[i].x = (float)(rand() % 100 - 50) / 500.0f;  // Was /100.0f
			velocities[i].y = (float)(rand() % 100 - 50) / 500.0f;  // Was /100.0f

			// Set random targets
			targets[i].x = (float)(rand() % (int)screenSize.x);
			targets[i].y = (float)(rand() % (int)screenSize.y);
		}
		initialized = true;
	}

	// Update particles with gentle movement
	for (int i = 0; i < particleCount; i++) {
		// Calculate direction to target
		float dx = targets[i].x - particles[i].x;
		float dy = targets[i].y - particles[i].y;
		float distance = sqrtf(dx * dx + dy * dy);

		// Very gentle acceleration toward target
		if (distance > 10.0f) {
			float speedFactor = 0.02f; // Very slow movement factor
			velocities[i].x += (dx / distance) * speedFactor;
			velocities[i].y += (dy / distance) * speedFactor;
		}
		else {
			// Assign new random target when close
			targets[i].x = (float)(rand() % (int)screenSize.x);
			targets[i].y = (float)(rand() % (int)screenSize.y);
		}

		// Apply gentle friction
		velocities[i].x *= 0.98f;
		velocities[i].y *= 0.98f;

		// Move particles
		particles[i].x += velocities[i].x;
		particles[i].y += velocities[i].y;

		// Soft bounce off screen edges
		if (particles[i].x < 0 || particles[i].x > screenSize.x) {
			velocities[i].x *= -0.7f; // Gentle bounce
			particles[i].x = std::max(0.0f, std::min(particles[i].x, screenSize.x));
		}

		if (particles[i].y < 0 || particles[i].y > screenSize.y) {
			velocities[i].y *= -0.7f; // Gentle bounce
			particles[i].y = std::max(0.0f, std::min(particles[i].y, screenSize.y));
		}

		// Very slow random velocity changes
		if (rand() % 100 < 2) {  // Was <5 (less frequent changes)
			velocities[i].x += (float)(rand() % 100 - 50) / 1000.0f;  // Was /200.0f
			velocities[i].y += (float)(rand() % 100 - 50) / 1000.0f;  // Was /200.0f
		}

		velocities[i].x *= 0.95f;  // 5% friction per frame (higher = more friction)
		velocities[i].y *= 0.95f;

		// Very low speed limit
		float speed = sqrtf(velocities[i].x * velocities[i].x + velocities[i].y * velocities[i].y);
		// Reduce this value (smaller = slower)
		if (speed > 0.5f) {  // Was 1.5f
			velocities[i].x = velocities[i].x / speed * 0.5f;  // Was *1.5f
			velocities[i].y = velocities[i].y / speed * 0.5f;  // Was *1.5f
		}
		
	}

	// Draw connections with gentle fading
	ID2D1SolidColorBrush* colorBrush = getSolidColorBrush(color);
	for (int i = 0; i < particleCount; i++) {
		for (int j = i + 1; j < particleCount; j++) {
			float dx = particles[i].x - particles[j].x;
			float dy = particles[i].y - particles[j].y;
			float distance = sqrtf(dx * dx + dy * dy);

			if (distance < maxDistance) {
				// Soft alpha based on distance
				float alpha = (1.0f - (distance / maxDistance)) * 0.6f; // More transparent
				UIColor lineColor(color.r, color.g, color.b, (uint8_t)(alpha * color.a));

				ID2D1SolidColorBrush* lineBrush = getSolidColorBrush(lineColor);
				d2dDeviceContext->DrawLine(
					D2D1::Point2F(particles[i].x, particles[i].y),
					D2D1::Point2F(particles[j].x, particles[j].y),
					lineBrush,
					lineWidth * 0.7f // Thinner lines
				);
			}
		}

		// Draw the particle itself very subtly
		UIColor particleColor(color.r, color.g, color.b, (uint8_t)(color.a * 0.7f));
		fillCircle(particles[i], particleColor, 1.5f); // Smaller particles
	}
}


void D2D::drawRainParticles(const Vec2<float>& screenSize, const UIColor& color, int particleCount, float speed, float length) {
	// Create particles if they don't exist
	static std::vector<Vec2<float>> particles;
	static bool initialized = false;

	if (!initialized) {
		particles.resize(particleCount);

		// Initialize particles randomly across the top of the screen
		for (int i = 0; i < particleCount; i++) {
			particles[i].x = (float)(rand() % (int)screenSize.x);
			particles[i].y = (float)(rand() % (int)screenSize.y) - screenSize.y; // Start above the screen
		}
		initialized = true;
	}

	ID2D1SolidColorBrush* colorBrush = getSolidColorBrush(color);

	// Update and draw particles
	for (int i = 0; i < particleCount; i++) {
		// Move particle down
		particles[i].y += speed * deltaTime * 60.f; // Adjust for framerate

		// If particle goes below screen, reset it to the top with a new x position
		if (particles[i].y > screenSize.y) {
			particles[i].x = (float)(rand() % (int)screenSize.x);
			particles[i].y = (float)(rand() % 100) - 100.f; // Start above the screen
		}

		// Calculate end position (bottom of rain drop)
		Vec2<float> endPos = particles[i];
		endPos.y += length;

		// Draw rain drop (line)
		d2dDeviceContext->DrawLine(
			D2D1::Point2F(particles[i].x, particles[i].y),
			D2D1::Point2F(endPos.x, endPos.y),
			colorBrush,
			1.0f
		);

		// Draw small splash at the bottom (optional)
		if (endPos.y >= screenSize.y - 5.f) {
			float splashSize = 2.f;
			d2dDeviceContext->DrawLine(
				D2D1::Point2F(endPos.x - splashSize, endPos.y),
				D2D1::Point2F(endPos.x + splashSize, endPos.y),
				colorBrush,
				1.0f
			);
		}
	}
}

std::wstring to_wide(const std::string& str) {
	return std::wstring(str.begin(), str.end());
}

uint64_t getTextLayoutKey(const std::string& textStr, float textSize) {
	std::hash<std::string> textHash;
	std::hash<float> textSizeHash;
	uint64_t combinedHash = textHash(textStr) ^ textSizeHash(textSize);
	return combinedHash;
}

IDWriteTextFormat* getTextFormat(float textSize) {
	if (textFormatCache[textSize].get() == nullptr) {
		std::wstring fontNameWide = to_wide(currentD2DFont);
		const WCHAR* fontName = fontNameWide.c_str();
		d2dWriteFactory->CreateTextFormat(
			fontName,
			nullptr,
			DWRITE_FONT_WEIGHT_NORMAL,
			isFontItalic ? DWRITE_FONT_STYLE_ITALIC : DWRITE_FONT_STYLE_NORMAL,		//DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			(float)currentD2DFontSize * textSize,
			L"en-us", //locale
			textFormatCache[textSize].put()
		);
	}

	return textFormatCache[textSize].get();
}

IDWriteTextLayout* getTextLayout(const std::string& textStr, float textSize, bool storeTextLayout) {

	std::wstring wideText = to_wide(textStr);
	const WCHAR* text = wideText.c_str();
	IDWriteTextFormat* textFormat = getTextFormat(textSize);
	uint64_t textLayoutKey = getTextLayoutKey(textStr, textSize);

	if (storeTextLayout) {
		if (textLayoutCache[textLayoutKey].get() == nullptr) {
			d2dWriteFactory->CreateTextLayout(
				text,
				(UINT32)wcslen(text),
				textFormat,
				FLT_MAX,
				0.f,
				textLayoutCache[textLayoutKey].put()
			);
		}
		return textLayoutCache[textLayoutKey].get();
	}
	else {
		if (textLayoutTemporary[textLayoutKey].get() == nullptr) {
			d2dWriteFactory->CreateTextLayout(
				text,
				(UINT32)wcslen(text),
				textFormat,
				FLT_MAX,
				0.f,
				textLayoutTemporary[textLayoutKey].put()
			);
		}
		return textLayoutTemporary[textLayoutKey].get();
	}
}

ID2D1SolidColorBrush* getSolidColorBrush(const UIColor& color) {
	uint32_t colorBrushKey = ColorUtil::ColorToUInt(color);
	if (colorBrushCache[colorBrushKey].get() == nullptr) {
		d2dDeviceContext->CreateSolidColorBrush(color.toD2D1Color(), colorBrushCache[colorBrushKey].put());
	}
	return colorBrushCache[colorBrushKey].get();
}

#include <wincodec.h> 
#pragma comment(lib, "windowscodecs.lib") 

IWICImagingFactory* wicFactory = nullptr;

void D2D::InitWICFactory() {
	if (!wicFactory) {
		HRESULT hr = CoCreateInstance(
			CLSID_WICImagingFactory,
			nullptr,
			CLSCTX_INPROC_SERVER,
			IID_PPV_ARGS(&wicFactory)
		);
		if (FAILED(hr)) {
		}
	}
}

void D2D::CleanupWICFactory() {
	if (wicFactory) {
		wicFactory->Release();
		wicFactory = nullptr;
	}
}
ID2D1Bitmap* D2D::loadBitmapFromFile(const std::wstring& filePath) {
	IWICBitmapDecoder* decoder = nullptr;
	IWICBitmapFrameDecode* frame = nullptr;
	IWICFormatConverter* converter = nullptr;
	ID2D1Bitmap* bitmap = nullptr;

	HRESULT hr = wicFactory->CreateDecoderFromFilename(
		filePath.c_str(),
		nullptr,
		GENERIC_READ,
		WICDecodeMetadataCacheOnLoad,
		&decoder
	);
	if (FAILED(hr)) return nullptr;

	hr = decoder->GetFrame(0, &frame);
	if (FAILED(hr)) {
		decoder->Release();
		return nullptr;
	}

	hr = wicFactory->CreateFormatConverter(&converter);
	if (FAILED(hr)) {
		frame->Release();
		decoder->Release();
		return nullptr;
	}

	hr = converter->Initialize(
		frame,
		GUID_WICPixelFormat32bppPBGRA,
		WICBitmapDitherTypeNone,
		nullptr,
		0.f,
		WICBitmapPaletteTypeMedianCut
	);
	if (FAILED(hr)) {
		converter->Release();
		frame->Release();
		decoder->Release();
		return nullptr;
	}

	hr = d2dDeviceContext->CreateBitmapFromWicBitmap(converter, nullptr, &bitmap);

	converter->Release();
	frame->Release();
	decoder->Release();

	if (FAILED(hr)) return nullptr;

	return bitmap;
}

void D2D::drawImageAt3DPos(const Vec3<float>& worldPos, ID2D1Bitmap* bitmap, float size) {
	Vec2<float> screenPos;
	if (!MCR::worldToScreen(worldPos, screenPos)) {
		return;
	}
	float halfSize = size / 2.0f;
	D2D1_RECT_F destRect = D2D1::RectF(
		screenPos.x - halfSize,
		screenPos.y - halfSize,
		screenPos.x + halfSize,
		screenPos.y + halfSize
	);
	d2dDeviceContext->DrawBitmap(bitmap, destRect);
}




#include <wincodec.h> 
#pragma comment(lib, "windowscodecs.lib")
#include <filesystem> 
ID2D1Bitmap* nameBitmap = nullptr;
#include "../Utils/FileUtil.h"

void loadtest() {
	if (nameBitmap != nullptr) return;
	std::string imagePath = FileUtil::getClientPath() + "Configs\\image.png";
	if (!std::filesystem::exists(imagePath)) {
		return;
	}
	std::wstring widePath = to_wide(imagePath);

	IWICBitmapDecoder* decoder = nullptr;
	IWICBitmapFrameDecode* frame = nullptr;
	IWICFormatConverter* converter = nullptr;
	IWICImagingFactory* wicFactory = nullptr;

	CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	HRESULT hr = CoCreateInstance(
		CLSID_WICImagingFactory, nullptr,
		CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&wicFactory)
	);

	if (SUCCEEDED(hr)) {
		hr = wicFactory->CreateDecoderFromFilename(
			widePath.c_str(),
			nullptr,
			GENERIC_READ,
			WICDecodeMetadataCacheOnLoad,
			&decoder
		);
	}

	if (SUCCEEDED(hr)) {
		hr = decoder->GetFrame(0, &frame);
	}

	if (SUCCEEDED(hr)) {
		hr = wicFactory->CreateFormatConverter(&converter);
	}

	if (SUCCEEDED(hr)) {
		hr = converter->Initialize(
			frame,
			GUID_WICPixelFormat32bppPBGRA,
			WICBitmapDitherTypeNone,
			nullptr, 0.0,
			WICBitmapPaletteTypeCustom
		);
	}

	if (SUCCEEDED(hr)) {
		hr = d2dDeviceContext->CreateBitmapFromWicBitmap(
			converter,
			nullptr,
			&nameBitmap
		);
	}

	if (converter) converter->Release();
	if (frame) frame->Release();
	if (decoder) decoder->Release();
	if (wicFactory) wicFactory->Release();
}
void D2D::drawNamePng(float x, float y, float width, float height) {
	if (!nameBitmap) {
		loadtest();
		if (!nameBitmap) return;
	}

	D2D1_RECT_F destRect = D2D1::RectF(x, y, x + width, y + height);
	d2dDeviceContext->DrawBitmap(nameBitmap, destRect);
}
void unload() {
	if (nameBitmap) {
		nameBitmap->Release();
		nameBitmap = nullptr;
	}
}

ID2D1BitmapRenderTarget* glowRenderTarget = nullptr;
ID2D1SolidColorBrush* glowBrush = nullptr;
ID2D1Effect* blurEffect2 = nullptr;
ID2D1Effect* transformEffect = nullptr;

void D2D::drawGlowingText(const Vec2<float>& textPos, const std::string& textStr, const UIColor& color, float textSize, float glowSize) {
	IDWriteTextLayout* textLayout = getTextLayout(textStr, textSize, false);
	DWRITE_TEXT_METRICS textMetrics;
	textLayout->GetMetrics(&textMetrics);
	float width = textMetrics.width + glowSize * 4;
	float height = textMetrics.height + glowSize * 4;
	if (!glowRenderTarget || glowRenderTarget->GetSize().width < width || glowRenderTarget->GetSize().height < height) {
		if (glowRenderTarget) glowRenderTarget->Release();

		d2dDeviceContext->CreateCompatibleRenderTarget(
			D2D1::SizeF(width, height),
			&glowRenderTarget
		);
	}
	if (!glowBrush) {
		d2dDeviceContext->CreateSolidColorBrush(
			D2D1::ColorF(color.r / 255.f, color.g / 255.f, color.b / 255.f, 1.0f),
			&glowBrush
		);
	}
	else {
		glowBrush->SetColor(D2D1::ColorF(color.r / 255.f, color.g / 255.f, color.b / 255.f, 1.0f));
	}
	glowRenderTarget->BeginDraw();
	glowRenderTarget->Clear(D2D1::ColorF(0, 0, 0, 0));
	glowRenderTarget->DrawTextLayout(
		D2D1::Point2F(glowSize * 2, glowSize * 2),
		textLayout,
		glowBrush
	);
	glowRenderTarget->EndDraw();
	ID2D1Bitmap* glowBitmap = nullptr;
	glowRenderTarget->GetBitmap(&glowBitmap);
	if (!blurEffect2) {
		d2dDeviceContext->CreateEffect(CLSID_D2D1GaussianBlur, &blurEffect2);
	}
	blurEffect2->SetInput(0, glowBitmap);
	blurEffect2->SetValue(D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION, glowSize);
	if (!transformEffect) {
		d2dDeviceContext->CreateEffect(CLSID_D2D12DAffineTransform, &transformEffect);
	}
	transformEffect->SetInputEffect(0, blurEffect2);
	D2D1_MATRIX_3X2_F matrix = D2D1::Matrix3x2F::Translation(textPos.x - glowSize * 2, textPos.y - glowSize * 2);
	transformEffect->SetValue(D2D1_2DAFFINETRANSFORM_PROP_TRANSFORM_MATRIX, matrix);
	d2dDeviceContext->DrawImage(transformEffect);
	ID2D1SolidColorBrush* colorBrush = getSolidColorBrush(color);
	d2dDeviceContext->DrawTextLayout(
		D2D1::Point2F(textPos.x, textPos.y),
		textLayout,
		colorBrush
	);
	glowBitmap->Release();
}

void D2D::drawStarParticles(const Vec2<float>& screenSize, const UIColor& color, int particleCount, float speed, float size) {
	// Create particles if they don't exist
	static std::vector<Vec2<float>> particles;
	static std::vector<float> sizes;
	static std::vector<float> opacities;
	static std::vector<float> angles;
	static std::vector<float> rotationSpeeds;
	static bool initialized = false;

	if (!initialized) {
		particles.resize(particleCount);
		sizes.resize(particleCount);
		opacities.resize(particleCount);
		angles.resize(particleCount);
		rotationSpeeds.resize(particleCount);

		// Initialize particles randomly across the screen
		for (int i = 0; i < particleCount; i++) {
			particles[i].x = (float)(rand() % (int)screenSize.x);
			particles[i].y = (float)(rand() % (int)screenSize.y);
			sizes[i] = (float)(rand() % (int)(size * 100)) / 100.0f + 0.5f; // Random size between 0.5 and size+0.5
			opacities[i] = (float)(rand() % 70 + 30) / 100.0f; // Random opacity between 0.3 and 1.0
			angles[i] = (float)(rand() % 360); // Random initial angle
			rotationSpeeds[i] = (float)(rand() % 20 - 10) / 10.0f; // Random rotation speed between -1.0 and 1.0
		}
		initialized = true;
	}

	// Update and draw particles
	for (int i = 0; i < particleCount; i++) {
		// Update rotation angle
		angles[i] += rotationSpeeds[i];
		if (angles[i] > 360.0f) angles[i] -= 360.0f;
		if (angles[i] < 0.0f) angles[i] += 360.0f;

		// Move particle slightly (twinkling effect)
		if (rand() % 100 < 5) { // 5% chance to move each frame
			particles[i].x += (float)(rand() % 5 - 2) * speed;
			particles[i].y += (float)(rand() % 5 - 2) * speed;

			// Keep particles within screen bounds
			particles[i].x = std::max(0.0f, std::min(particles[i].x, screenSize.x));
			particles[i].y = std::max(0.0f, std::min(particles[i].y, screenSize.y));
		}

		// Randomly change opacity for twinkling effect
		if (rand() % 100 < 10) { // 10% chance to change opacity each frame
			opacities[i] = (float)(rand() % 70 + 30) / 100.0f;
		}

		// Create color with current opacity
		UIColor starColor(color.r, color.g, color.b, (uint8_t)(color.a * opacities[i]));
		ID2D1SolidColorBrush* starBrush = getSolidColorBrush(starColor);

		// Draw star (rotated square)
		float halfSize = sizes[i] / 2.0f;
		float centerX = particles[i].x;
		float centerY = particles[i].y;

		// Calculate rotated points for the star
		float rad = angles[i] * (3.14159265f / 180.0f);
		float cosA = cosf(rad);
		float sinA = sinf(rad);

		// Star points (4-point star)
		D2D1_POINT_2F points[8];
		for (int j = 0; j < 4; j++) {
			float angle = (float)j * (3.14159265f / 2.0f) + rad;
			float x1 = centerX + cosf(angle) * halfSize;
			float y1 = centerY + sinf(angle) * halfSize;
			float x2 = centerX + cosf(angle) * (halfSize * 0.5f);
			float y2 = centerY + sinf(angle) * (halfSize * 0.5f);

			points[j * 2] = D2D1::Point2F(x1, y1);
			points[j * 2 + 1] = D2D1::Point2F(x2, y2);
		}

		// Draw the star
		d2dDeviceContext->DrawLine(points[0], points[1], starBrush, 1.0f);
		d2dDeviceContext->DrawLine(points[1], points[2], starBrush, 1.0f);
		d2dDeviceContext->DrawLine(points[2], points[3], starBrush, 1.0f);
		d2dDeviceContext->DrawLine(points[3], points[4], starBrush, 1.0f);
		d2dDeviceContext->DrawLine(points[4], points[5], starBrush, 1.0f);
		d2dDeviceContext->DrawLine(points[5], points[6], starBrush, 1.0f);
		d2dDeviceContext->DrawLine(points[6], points[7], starBrush, 1.0f);
		d2dDeviceContext->DrawLine(points[7], points[0], starBrush, 1.0f);
	}
}

void D2D::fillRoundedRectangle(const Vec4<float>& rect, const UIColor& color, float radius) {
	ID2D1SolidColorBrush* colorBrush = getSolidColorBrush(color);
	D2D1_ROUNDED_RECT roundedRect = D2D1::RoundedRect(
		D2D1::RectF(rect.x, rect.y, rect.z, rect.w),
		radius,
		radius
	);
	d2dDeviceContext->FillRoundedRectangle(roundedRect, colorBrush);
}

void D2D::drawTriangle(const Vec2<float>& p1, const Vec2<float>& p2, const Vec2<float>& p3, const UIColor& color, float width) {
	ID2D1PathGeometry* geometry = nullptr;
	d2dFactory->CreatePathGeometry(&geometry);

	ID2D1GeometrySink* sink = nullptr;
	geometry->Open(&sink);

	sink->BeginFigure(D2D1::Point2F(p1.x, p1.y), D2D1_FIGURE_BEGIN_FILLED);
	sink->AddLine(D2D1::Point2F(p2.x, p2.y));
	sink->AddLine(D2D1::Point2F(p3.x, p3.y));
	sink->EndFigure(D2D1_FIGURE_END_CLOSED);

	sink->Close();
	sink->Release();

	ID2D1SolidColorBrush* colorBrush = getSolidColorBrush(color);
	d2dDeviceContext->DrawGeometry(geometry, colorBrush, width);

	geometry->Release();
}

void D2D::fillTriangle(const Vec2<float>& p1, const Vec2<float>& p2, const Vec2<float>& p3, const UIColor& color) {
	ID2D1PathGeometry* geometry = nullptr;
	d2dFactory->CreatePathGeometry(&geometry);

	ID2D1GeometrySink* sink = nullptr;
	geometry->Open(&sink);

	sink->BeginFigure(D2D1::Point2F(p1.x, p1.y), D2D1_FIGURE_BEGIN_FILLED);
	sink->AddLine(D2D1::Point2F(p2.x, p2.y));
	sink->AddLine(D2D1::Point2F(p3.x, p3.y));
	sink->EndFigure(D2D1_FIGURE_END_CLOSED);

	sink->Close();
	sink->Release();

	ID2D1SolidColorBrush* colorBrush = getSolidColorBrush(color);
	d2dDeviceContext->FillGeometry(geometry, colorBrush);

	geometry->Release();
}