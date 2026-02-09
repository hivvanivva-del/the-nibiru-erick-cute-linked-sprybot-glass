#include "ModuleManager.h"
#include "../../Client.h"

void ModuleManager::init() {

	//COMBAT
	moduleList.push_back(new BowSpam());
	moduleList.push_back(new AntiPiston());
	moduleList.push_back(new MaceKiller());
	moduleList.push_back(new TargetDown());
	moduleList.push_back(new PistonPush());
	moduleList.push_back(new BedAura());
	moduleList.push_back(new AutoCity());
	moduleList.push_back(new AnchorAura());
	moduleList.push_back(new AutoCrystal());
	moduleList.push_back(new AutoCrystal2());
	moduleList.push_back(new Criticals());
	moduleList.push_back(new AutoTrap());
	moduleList.push_back(new SelfTrap());
	moduleList.push_back(new Hitbox());
	moduleList.push_back(new KillAura());
	moduleList.push_back(new Surround());
	moduleList.push_back(new KillAura2());
	moduleList.push_back(new KillauraTRI());

	//MOVEMENT
	moduleList.push_back(new AirJump());
	moduleList.push_back(new ElytraFly());
	moduleList.push_back(new InventoryMove());
	moduleList.push_back(new AutoWalk());
	moduleList.push_back(new MovementFix());
	moduleList.push_back(new FastStop());
	moduleList.push_back(new LiquidMove());
	moduleList.push_back(new ReverseStep());
	moduleList.push_back(new Speed());
	moduleList.push_back(new Clip());
	moduleList.push_back(new NoClip());
	moduleList.push_back(new AutoSprint());
	moduleList.push_back(new Fly());
	moduleList.push_back(new Jetpack());
	moduleList.push_back(new NoSlowDown());
	moduleList.push_back(new Phase());
	moduleList.push_back(new Spider());
	moduleList.push_back(new Velocity());
	moduleList.push_back(new Purplez());

	//RENDER
	moduleList.push_back(new Arraylist());
	moduleList.push_back(new StorageESP());
	moduleList.push_back(new TunnelESP());
	moduleList.push_back(new PortalESP());
	moduleList.push_back(new CrystalChams());
	moduleList.push_back(new Tint());
	moduleList.push_back(new BlockHighlight());
	moduleList.push_back(new BreakIndicators());
	moduleList.push_back(new CameraTweaks());
	moduleList.push_back(new ChunkBorders());
	moduleList.push_back(new CustomFov());
	moduleList.push_back(new ESP());
	moduleList.push_back(new FreeLook());
	moduleList.push_back(new Fullbright());
	moduleList.push_back(new GlintColor());
	moduleList.push_back(new HoleESP());
	moduleList.push_back(new HUD());
	moduleList.push_back(new NameTags());
	moduleList.push_back(new NewChunks());
	moduleList.push_back(new NoHurtCam());
	moduleList.push_back(new NoRender());
	moduleList.push_back(new Swing());
	moduleList.push_back(new Tracer());
	moduleList.push_back(new ViewModel());
	moduleList.push_back(new Stats());
	moduleList.push_back(new InventoryView());
	moduleList.push_back(new PlayerChams());
	moduleList.push_back(new PlayerList());
	moduleList.push_back(new Minimap());

	//PLAYER
	moduleList.push_back(new AutoTool());
	moduleList.push_back(new AntiCrystal());
	moduleList.push_back(new PopCounter());
	moduleList.push_back(new BlockReach());
	moduleList.push_back(new PacketMine());
	moduleList.push_back(new HotbarSorter());
	moduleList.push_back(new FastUse());
	moduleList.push_back(new NoSwing());
	moduleList.push_back(new Offhand());
	moduleList.push_back(new FreeCam());
	

	//WORLD
	moduleList.push_back(new AirPlace());
	moduleList.push_back(new DeathLogger());
	moduleList.push_back(new Scaffold());
	moduleList.push_back(new AutoWither());
	moduleList.push_back(new AutoTunnel());
	moduleList.push_back(new NoEntityTrace());
	moduleList.push_back(new BasePlace());
	moduleList.push_back(new AutoHoleFill());
	moduleList.push_back(new PacketHand());
	moduleList.push_back(new SpiderTower());
	moduleList.push_back(new CrystalPlacer());
	moduleList.push_back(new AutoMapart());
	moduleList.push_back(new ClientSideInv());
	moduleList.push_back(new Franky());
	moduleList.push_back(new Crasher());
	moduleList.push_back(new PortalGodMode());
	moduleList.push_back(new Trident());
	moduleList.push_back(new Reach());
	moduleList.push_back(new PacketCriticals());
	moduleList.push_back(new Flags());

	//MISC
	moduleList.push_back(new Disabler());
	moduleList.push_back(new AutoLog());
	moduleList.push_back(new AutoEmote());
	moduleList.push_back(new AntiDesync());
	moduleList.push_back(new ChatSuffix());
	moduleList.push_back(new AutoKit());
	moduleList.push_back(new NoPacket());
	moduleList.push_back(new Timer());
	moduleList.push_back(new ChatSpammer());
	moduleList.push_back(new TickShift());
	moduleList.push_back(new AutoHighway());
	//moduleList.push_back(new NoFall());

	//CLIENT
	moduleList.push_back(new ClickGUI());
	moduleList.push_back(new Colors());
	moduleList.push_back(new CustomFont());
	moduleList.push_back(new TestModule());

	std::sort(moduleList.begin(), moduleList.end(), [](Module* lhs, Module* rhs) {
		return lhs->getModuleName() < rhs->getModuleName();
	});

	getModule<ClickGUI>()->InitClickGUI();

	getModule<Arraylist>()->setEnabled(true);
	getModule<HUD>()->setEnabled(true);
}

void ModuleManager::shutdown() {
	for (auto& mod : moduleList) {
		mod->setEnabled(false);
	}
	for (auto& mod : moduleList) {
		delete mod;
		mod = nullptr;
	} 
	moduleList.clear();
}

void ModuleManager::onKeyUpdate(int key, bool isDown) {
	if (!Client::isInitialized())
		return;

	for (auto& mod : moduleList) {
		mod->onKeyUpdate(key, isDown);
	}
}

void ModuleManager::onClientTick() {
	if (!Client::isInitialized())
		return;

	for (auto& mod : moduleList) {
		if (mod->isEnabled() || mod->runOnBackground()) {
			mod->onClientTick();
		}
	}
}

void ModuleManager::onNormalTick(LocalPlayer* localPlayer) {
	if (!Client::isInitialized()) return;
	if (!localPlayer->level) return;
	for (auto& mod : moduleList) {
		if (mod->isEnabled() || mod->runOnBackground()) {
			mod->onNormalTick(localPlayer);
		}
	}
}

void ModuleManager::onLevelTick(Level* level) {
	if (!Client::isInitialized())
		return;

	for (auto& mod : moduleList) {
		if (mod->isEnabled() || mod->runOnBackground()) {
			mod->onLevelTick(level);
		}
	}
}

void ModuleManager::onUpdateRotation(LocalPlayer* localPlayer) {
	if (!Client::isInitialized())
		return;

	for (auto& mod : moduleList) {
		if (mod->isEnabled() || mod->runOnBackground()) {
			mod->onUpdateRotation(localPlayer);
		}
	}
}

void ModuleManager::onSendPacket(Packet* packet, bool& cancel) {
	if (!Client::isInitialized())
		return;

	for (auto& mod : moduleList) {
		if (mod->isEnabled() || mod->runOnBackground()) {
			mod->onSendPacket(packet, cancel);
		}
		if (cancel) break;
	}
}


void ModuleManager::onRecievePacket(Packet* packet, bool& cancel) {
	if (!Client::isInitialized())
		return;

	for (auto& mod : moduleList) {
		if (mod->isEnabled() || mod->runOnBackground()) {
			mod->onRecievePacket(packet, cancel);
		}
		if (cancel) break;
	}
}

void ModuleManager::onD2DRender() {
	if (!Client::isInitialized())
		return;

	for (auto& mod : moduleList) {
		if (mod->isEnabled() || mod->runOnBackground()) {
			mod->onD2DRender();
		}
	}
}

void ModuleManager::onMCRender(MinecraftUIRenderContext* renderCtx) {
	if (!Client::isInitialized())
		return;

	for (auto& mod : moduleList) {
		if (mod->isEnabled() || mod->runOnBackground()) {
			mod->onMCRender(renderCtx);
		}
	}
}

void ModuleManager::onLevelRender() {
	if (!Client::isInitialized())
		return;

	for (auto& mod : moduleList) {
		if (mod->isEnabled() || mod->runOnBackground()) {
			mod->onLevelRender();
		}
	}
}

void ModuleManager::onLoadConfig(void* conf) {
	if (!Client::isInitialized())
		return;

	for (auto& mod : moduleList) {
		mod->onLoadConfig(conf);
	}
}

void ModuleManager::onSaveConfig(void* conf) {
	if (!Client::isInitialized())
		return;

	for (auto& mod : moduleList) {
		mod->onSaveConfig(conf);
	}
}

void ModuleManager::onAttack(Actor* actor, bool& cancel) {
	if (!Client::isInitialized()) return;
	for (auto& mod : moduleList) {
		if (mod->isEnabled() || mod->runOnBackground()) {
			mod->onAttack(actor, cancel);
		}
		if (cancel) break;
	}
}

void ModuleManager::onBuild(const BlockPos& blockPos, const uint8_t face, bool& cancel) {
	if (!Client::isInitialized()) return;
	for (auto& mod : moduleList) {
		if (mod->isEnabled() || mod->runOnBackground()) {
			mod->onBuild(blockPos, face, cancel);
		}
		if (cancel) break;
	}
}