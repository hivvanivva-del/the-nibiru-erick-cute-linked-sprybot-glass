#include "Client.h"

#include "../Utils/FileUtil.h"
#include "../Utils/NetworkUtil.h"
#include "../Utils/Logger.h"

void AddFont(const std::string& fontPath) {
	std::wstring temp = std::wstring(fontPath.begin(), fontPath.end());
	AddFontResource(temp.c_str());
}

void Client::DisplayClientMessage(const char* fmt, ...) {
	LocalPlayer* localPlayer = Game::getLocalPlayer();
	if (localPlayer == nullptr)
		return;

	va_list arg;
	va_start(arg, fmt);
	char message[300];
	vsprintf_s(message, 300, fmt, arg);
	va_end(arg);

	std::string headerMessage;
	if (headerMessage.empty()) {
		char headerCStr[50];
		sprintf_s(headerCStr, 50, "%s[%s%s%s] %s", MCTF::GRAY, MCTF::BLUE, getClientName().c_str(), MCTF::GRAY, MCTF::WHITE);
		headerMessage = std::string(headerCStr);
	}

	std::string messageStr = headerMessage + std::string(message);
	localPlayer->displayClientMessage(messageStr);
}

void Client::init() {
	if (!NetworkUtil::canAccessInternet()) {
		abort();
		return;
	}
	
	std::string ClientPath = FileUtil::getClientPath();
	if (!FileUtil::doesFilePathExist(ClientPath)) {
		FileUtil::createPath(ClientPath);
	}
	else {
		if (!FileUtil::doesFilePathExist(ClientPath + "Maparts\\")) {
			FileUtil::createPath(ClientPath + "Maparts\\");
		}
	}
	Logger::init();
	ConfigManager::init();
	
	/*
	
	
	std::string FontsPath = ClientPath + "Fonts\\";
	if (!FileUtil::doesFilePathExist(FontsPath)) {
		FileUtil::createPath(FontsPath);
	}
	
	std::string ProductSansFontPath = FontsPath + "ProductSans.ttf";
	if (!FileUtil::doesFilePathExist(ProductSansFontPath)) {
		NetworkUtil::downloadFile("ProductSans.ttf", FontsPath, "https://github.com/MerciGG/Font-Repository/releases/download/v1/Product.Sans.Regular.ttf");
	}

	AddFont(ProductSansFontPath);
	*/
	Addresses::init();
	MCTF::initMCTF();

	ModuleManager::init();
	CommandManager::init();
	HookManager::init();

	if (Game::clientInstance == nullptr)
		return;
	/*
	logF("%llx", Game::clientInstance);
	logF("%llx", Game::clientInstance->mcGame);
	logF("%llx", Game::getLocalPlayer());
	*/

	initialized = true;
	NotificationManager::addNotification("Successfully injected Nibiru Client!", 5.f);

	if (!ConfigManager::doesConfigExist("default"))
		ConfigManager::createNewConfig("default");

	ConfigManager::loadConfig("default");

}

void Client::shutdown() {
	ConfigManager::saveConfig();
	initialized = false;
}