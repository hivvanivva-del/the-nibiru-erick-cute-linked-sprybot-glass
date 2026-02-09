#pragma once
#include "../../ModuleBase/Module.h"
#include "../../../../../../Libs/json.hpp"

using json = nlohmann::json;

class AutoKit : public Module {
public:
	std::string firstKit = "1";
	std::string secondKit = "2";
public:
	AutoKit() : Module("AutoKit", "Kits automatically!", Category::MISC) {}

	virtual void onNormalTick(LocalPlayer* localPlayer) override {
		static float delay = 0;
		if (!Game::canUseMoveKeys()) return;
		std::shared_ptr<Packet> sharedPkt = MinecraftPackets::createPacket(PacketID::CommandRequest);
		CommandRequestPacket* cmdPkt = (CommandRequestPacket*)sharedPkt.get();
		cmdPkt->isExternal = false;
		cmdPkt->origin = 0;
		if (TimerUtil::hasReached<ticks>(&delay, 25)) {
			if (localPlayer->getDimensionTypeComponent()->type == 0) cmdPkt->mCommand = "/kit " + firstKit;
			else cmdPkt->mCommand = "/kit " + secondKit;
			localPlayer->level->getPacketSender()->send(cmdPkt);
		}
	}
	virtual void onRecievePacket(Packet* packet, bool& shouldCancel) override {
		if (packet->getId() == PacketID::Text) {
			TextPacket* textPkt = (TextPacket*)packet;
			if (textPkt->mMessage.find("Kit used!") != std::string::npos) shouldCancel = true;
			if (textPkt->mMessage.find("Commands disabled in combat") != std::string::npos) shouldCancel = true;
			if (textPkt->mMessage.find("Kit loaded: ") != std::string::npos) shouldCancel = true;
			if (textPkt->mMessage.find("You can't use this command while in combat") != std::string::npos) shouldCancel = true;
		}
	}
	virtual void onLoadConfig(void* confVoid) override {
		json* config = reinterpret_cast<json*>(confVoid);
		std::string modName = this->getModuleName();
		try {
			if (config->contains(modName)) {
				json obj = config->at(getModuleName());
				if (obj.is_null()) return;
				if (obj.contains("enabled")) this->setEnabled(obj["enabled"].get<bool>());
				for (Setting* setting : this->getSettingList()) {
					if (obj.contains(setting->name)) {
						if (setting->type == SettingType::KEYBIND_S) {
							KeybindSetting* keySetting = (KeybindSetting*)setting;
							*keySetting->value = obj[keySetting->name].get<int>();
						}
						else if (setting->type == SettingType::BOOL_S) {
							BoolSetting* boolSetting = (BoolSetting*)setting;
							*boolSetting->value = obj[boolSetting->name].get<bool>();
						}
						else if (setting->type == SettingType::ENUM_S) {
							EnumSetting* enumSetting = (EnumSetting*)setting;
							*enumSetting->value = obj[enumSetting->name].get<int>();
						}
						else if (setting->type == SettingType::COLOR_S) {
							ColorSetting* colorSetting = (ColorSetting*)setting;
							json colorObj = obj[colorSetting->name];
							if (colorObj.contains("Red")) colorSetting->colorPtr->r = colorObj["Red"].get<int>();
							if (colorObj.contains("Green")) colorSetting->colorPtr->g = colorObj["Green"].get<int>();
							if (colorObj.contains("Blue")) colorSetting->colorPtr->b = colorObj["Blue"].get<int>();
							if (colorObj.contains("Alpha")) colorSetting->colorPtr->a = colorObj["Alpha"].get<int>();
							if (colorObj.contains("Synced")) colorSetting->colorSynced = colorObj["Synced"].get<bool>();
						}
						else if (setting->type == SettingType::SLIDER_S) {
							SliderSettingBase* sliderSetting = (SliderSettingBase*)setting;
							if (sliderSetting->valueType == ValueType::INT_T) {
								SliderSetting<int>* intSlider = (SliderSetting<int>*)sliderSetting;
								*intSlider->valuePtr = obj[intSlider->name].get<int>();
							}
							else if (sliderSetting->valueType == ValueType::FLOAT_T) {
								SliderSetting<float>* floatSlider = (SliderSetting<float>*)sliderSetting;
								*floatSlider->valuePtr = obj[floatSlider->name].get<float>();
							}
						}
					}
				}
				if (obj.contains("Kit Name")) {
					json confValue = obj.at("Kit Name");
					if (confValue.is_null()) return;

					if (confValue.contains("Overworld")) firstKit = confValue["Overworld"];
					if (confValue.contains("Nether")) secondKit = confValue["Nether"];
				}
			}
		}
		catch (const std::exception& e) {}
	}
	virtual void onSaveConfig(void* confVoid) override {
		json* currentConfig = reinterpret_cast<json*>(confVoid);
		try {
			json obj;
			obj["enabled"] = this->isEnabled();
			for (Setting* setting : this->getSettingList()) {
				if (setting->type == SettingType::KEYBIND_S) {
					KeybindSetting* keySetting = (KeybindSetting*)setting;
					obj[keySetting->name] = *keySetting->value;
				}
				else if (setting->type == SettingType::BOOL_S) {
					BoolSetting* boolSetting = (BoolSetting*)setting;
					obj[boolSetting->name] = *boolSetting->value;
				}
				else if (setting->type == SettingType::ENUM_S) {
					EnumSetting* enumSetting = (EnumSetting*)setting;
					obj[enumSetting->name] = *enumSetting->value;
				}
				else if (setting->type == SettingType::COLOR_S) {
					ColorSetting* colorSetting = (ColorSetting*)setting;
					obj[colorSetting->name]["Red"] = colorSetting->colorPtr->r;
					obj[colorSetting->name]["Green"] = colorSetting->colorPtr->g;
					obj[colorSetting->name]["Blue"] = colorSetting->colorPtr->b;
					obj[colorSetting->name]["Alpha"] = colorSetting->colorPtr->a;
					obj[colorSetting->name]["Synced"] = colorSetting->colorSynced;
				}
				else if (setting->type == SettingType::SLIDER_S) {
					SliderSettingBase* sliderSetting = (SliderSettingBase*)setting;
					if (sliderSetting->valueType == ValueType::INT_T) {
						SliderSetting<int>* intSlider = (SliderSetting<int>*)sliderSetting;
						obj[intSlider->name] = *intSlider->valuePtr;
					}
					else if (sliderSetting->valueType == ValueType::FLOAT_T) {
						SliderSetting<float>* floatSlider = (SliderSetting<float>*)sliderSetting;
						obj[floatSlider->name] = *floatSlider->valuePtr;
					}
				}
			}
			obj["Kit Name"]["Overworld"] = firstKit;
			obj["Kit Name"]["Nether"] = secondKit;
			(*currentConfig)[this->getModuleName()] = obj;
		}
		catch (const std::exception& e) {}
	}
};