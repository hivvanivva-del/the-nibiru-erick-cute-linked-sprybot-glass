#pragma once

//COMBAT
#include "Modules/Category/Combat/TargetDown.h"
#include "Modules/Category/Combat/MaceKiller.h"
#include "Modules/Category/Combat/PistonPush.h"
#include "Modules/Category/Combat/AutoCrystal.h"
#include "Modules/Category/Combat/AutoCrystal2.h"
#include "Modules/Category/Combat/AutoCity.h"
#include "Modules/Category/Combat/BedAura.h"
#include "Modules/Category/Combat/SelfTrap.h"
#include "Modules/Category/Combat/BowSpam.h"
#include "Modules/Category/Combat/AnchorAura.h"
#include "Modules/Category/Combat/Criticals.h"
#include "Modules/Category/Combat/AutoTrap.h"
#include "Modules/Category/Combat/AntiPiston.h"
#include "Modules/Category/Combat/Hitbox.h"
#include "Modules/Category/Combat/KillAura.h"
#include "Modules/Category/Combat/KillAura2.h"
#include "Modules/Category/Combat/Surround.h"
#include "Modules/Category/Combat/KillauraTRI.h"

//MOVEMENT
#include "Modules/Category/Movement/AirJump.h"
#include "Modules/Category/Movement/InventoryMove.h"
#include "Modules/Category/Movement/ElytraFly.h"
#include "Modules/Category/Movement/AutoWalk.h"
#include "Modules/Category/Movement/MovementFix.h"
#include "Modules/Category/Movement/LiquidMove.h"
#include "Modules/Category/Movement/ReverseStep.h"
#include "Modules/Category/Movement/FastStop.h"
#include "Modules/Category/Movement/AutoSprint.h"
#include "Modules/Category/Movement/Speed.h"
#include "Modules/Category/Movement/Clip.h"
#include "Modules/Category/Movement/Fly.h"
#include "Modules/Category/Movement/Jetpack.h"
#include "Modules/Category/Movement/NoSlowDown.h"
#include "Modules/Category/Movement/Phase.h"
#include "Modules/Category/Movement/Spider.h"
#include "Modules/Category/Movement/NoClip.h"
#include "Modules/Category/Movement/Velocity.h"
#include "Modules/Category/Movement/Purplez.h"

//RENDER
#include "Modules/Category/Render/Arraylist.h"
#include "Modules/Category/Render/StorageESP.h"
#include "Modules/Category/Render/TunnelESP.h"
#include "Modules/Category/Render/PortalESP.h"
#include "Modules/Category/Render/BlockHighlight.h"
#include "Modules/Category/Render/CrystalChams.h"
#include "Modules/Category/Render/Tint.h"
#include "Modules/Category/Render/BreakIndicators.h"
#include "Modules/Category/Render/CameraTweaks.h"
#include "Modules/Category/Render/ChunkBorders.h"
#include "Modules/Category/Render/CustomFov.h"
#include "Modules/Category/Render/ESP.h"
#include "Modules/Category/Render/FreeLook.h"
#include "Modules/Category/Render/Fullbright.h"
#include "Modules/Category/Render/GlintColor.h"
#include "Modules/Category/Render/HoleESP.h"
#include "Modules/Category/Render/HUD.h"
#include "Modules/Category/Render/NameTags.h"
#include "Modules/Category/Render/NewChunks.h"
#include "Modules/Category/Render/NoHurtCam.h"
#include "Modules/Category/Render/NoRender.h"
#include "Modules/Category/Render/Swing.h"
#include "Modules/Category/Render/Tracer.h"
#include "Modules/Category/Render/ViewModel.h"
#include "Modules/Category/Render/Stats.h"
#include "Modules/Category/Render/InvView.h"
#include "Modules/Category/Render/Chams.h"
#include "Modules/Category/Render/PlayerList.h"
#include "Modules/Category/Render/Minimap.h"

//PLAYER
#include "Modules/Category/Player/AutoTool.h"
#include "Modules/Category/Player/DeathLogger.h"
#include "Modules/Category/Player/HotbarSorter.h"
#include "Modules/Category/Player/AntiCrystal.h"
#include "Modules/Category/Player/PopCounter.h"
#include "Modules/Category/Player/PacketMine.h"
#include "Modules/Category/Player/BlockReach.h"
#include "Modules/Category/Player/FastUse.h"
#include "Modules/Category/Player/NoSwing.h"
#include "Modules/Category/Player/Offhand.h"
#include "Modules/Category/Player/FreeCam.h"

//WORLD
#include "Modules/Category/World/AirPlace.h"
#include "Modules/Category/World/AutoWither.h"
#include "Modules/Category/World/Scaffold.h"
#include "Modules/Category/World/AutoTunnel.h"
#include "Modules/Category/World/NoEntityTrace.h"
#include "Modules/Category/World/BasePlace.h"
#include "Modules/Category/World/AutoHoleFill.h"
#include "Modules/Category/World/PacketHand.h"
#include "Modules/Category/World/CrystalPlacer.h"
#include "Modules/Category/World/SpiderTower.h"
#include "Modules/Category/World/AutoMapart.h"
#include "Modules/Category/World/ClientSideInv.h"
#include "Modules/Category/World/Franky.h"
#include "Modules/Category/World/Crasher.h"
#include "Modules/Category/World/PortalGodMode.h"
#include "Modules/Category/World/Trident.h"
#include "Modules/Category/World/Reach.h"
#include "Modules/Category/World/PacketCriticals.hpp"
#include "Modules/Category/World/Flags.h"

//MISC
#include "Modules/Category/Misc/Disabler.h"
#include "Modules/Category/Misc/AutoEmote.h"
#include "Modules/Category/Misc/AutoLog.h"
#include "Modules/Category/Misc/NoPacket.h"
#include "Modules/Category/Misc/AntiDesync.h"
#include "Modules/Category/Misc/ChatSuffix.h"
#include "Modules/Category/Misc/AutoKit.h"
#include "Modules/Category/Misc/Timer.h"
#include "Modules/Category/Misc/TickShift.h"
#include "Modules/Category/Misc/Spammer.h"
#include "Modules/Category/Misc/AutoHighway.h"
#include "Modules/Category/Misc/NoFall.h"

//CLIENT
#include "Modules/Category/Client/ClickGUI.h"
#include "Modules/Category/Client/Colors.h"
#include "Modules/Category/Client/CustomFont.h"
#include "Modules/Category/Client/TestModule.h"

class ModuleManager {
public:
	static inline std::vector<Module*> moduleList;

	static void init();
	static void shutdown();

	template <typename TRet>
	static TRet* getModule() {
		for (Module* mod : moduleList) {
			TRet* result = dynamic_cast<TRet*>(mod);
			if (result == nullptr)
				continue;
			return result;
		}
		return nullptr;
	}

	static void onKeyUpdate(int key, bool isDown);
	static void onClientTick();
	static void onNormalTick(LocalPlayer* localPlayer);
	static void onLevelTick(Level* level);
	static void onUpdateRotation(LocalPlayer* localPlayer);
	static void onSendPacket(Packet* packet, bool& cancel);
	static void onRecievePacket(Packet* packet, bool& cancel);
	static void onD2DRender();
	static void onMCRender(MinecraftUIRenderContext* renderCtx);
	static void onLevelRender();
	static void onLoadConfig(void* conf);
	static void onSaveConfig(void* conf);
	
	// Events
	static void onAttack(Actor* actor, bool& cancel);
	static void onBuild(const BlockPos& blockPos, const uint8_t face, bool& cancel);
};