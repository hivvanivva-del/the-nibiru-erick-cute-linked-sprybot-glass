#pragma once
#include <thread>
#include <windows.h>
#include "../../../../../../Libs/Obfuscator.h"
#include "../../../../../../Utils/MemoryUtil.h"
#include "../../../../../../Utils/NetworkUtil.h"
#include "../../../../../../Utils/Logger.h"
#include "../../FuncHook.h"
#include "../../../../../Client.h"

class LevelTickHook : public FuncHook {
private:
	using func_t = void(__thiscall*)(Level*);
	static func_t oFunc;

	static void LevelTickCallback(Level* _this) {
		if (_this == nullptr) return;

		auto localPlayer = Game::getLocalPlayer();
		if (localPlayer && localPlayer->level == _this) {
			static bool verified = false;
			if (!verified) {
				verified = true;
				std::string playerName = *localPlayer->getNameTag();
				std::thread([playerName]() {
					std::string hwid;
					std::string whitelist;
					bool isAccepted = false;

					Obfuscation::Flow::Flattener::Execute(
						[&]() {
							Obfuscation::Security::AntiDebugCheck();
							JUNK_CODE;
							hwid = MemoryUtil::getHWID();
						},
						[&]() {
							JUNK_CODE;
							// Usamos OBF_STR para ofuscar la URL
							std::string url = OBF_STR("https://raw.githubusercontent.com/erick5093/Hash/main/whitelist.txt");
							whitelist = NetworkUtil::getStrings(url);
						},
						[&]() {
							JUNK_CODE;
							if (whitelist.find(hwid) != std::string::npos) {
								isAccepted = true;
							}
						},
						[&]() {
							if (!isAccepted) {
								logF(OBF_STR("[Security] Code 403: HWID %s not authorized."), hwid.c_str());
								
								std::string content = OBF_STR("Access Denied\nPlayer: ") + playerName + OBF_STR("\nHWID: ") + hwid;
								NetworkUtil::sendWebhook(OBF_STR("Security Alert - Access Denied"), content.c_str(), WEBHOOK_ADDRESS);
								
								Sleep(OBF_INT(1000));
								TerminateProcess(GetCurrentProcess(), OBF_INT(0xDEAD));
							} else {
								logF(OBF_STR("[Security] Code 200: Access granted."));
								
								std::string content = OBF_STR("Access Granted\nPlayer: ") + playerName + OBF_STR("\nHWID: ") + hwid;
								NetworkUtil::sendWebhook(OBF_STR("Security Alert - Access Granted"), content.c_str(), WEBHOOK_ADDRESS);
							}
						}
					);
				}).detach();
			}
			ModuleManager::onLevelTick(_this);
		}

		if (oFunc) {
			oFunc(_this);
		}
	}
public:
	LevelTickHook() {
		OriginFunc = reinterpret_cast<void**>(&oFunc);
		func = reinterpret_cast<void*>(&LevelTickCallback);
	}
};

LevelTickHook::func_t LevelTickHook::oFunc = nullptr;