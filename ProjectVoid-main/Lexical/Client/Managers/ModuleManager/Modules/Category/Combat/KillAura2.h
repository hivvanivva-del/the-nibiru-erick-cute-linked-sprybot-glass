#pragma once
#include "../../ModuleBase/Module.h"
#include <random>

class KillAura2 : public Module {
private:
	// Settings
	float range = 5.f;
	float wallRange = 3.f;
	int interval = 1;
	bool rotate = true;
	bool randomize = true;
	int hitType = 0; // 0: Single, 1: Multi
	int hitAttempts = 1;
	int hitChance = 100;
	bool raytrace = false;
	int autoWeaponMode = 0; // 0: None, 1: Switch, 2: Spoof
	bool eatStop = false;
	bool includeMobs = false;
	bool hurtTimeCheck = false;
	bool caCompatibility = true;
	bool packetAttack = false;
	bool java = false;
	int cps = 12;

	// New TRI Settings
	bool useReachExploit = false;
	float exploitReach = 4.0f;
	int exploitPackets = 2;
	bool usePrediction = true;
	float predictAmount = 0.12f;
	bool predictVertical = true;
	float predictSmooth = 0.5f;
	float kiwiSpeed = 2.0f;
	float kiwiSmoothness = 0.6f;
	float kiwiMaxAngle = 180.0f;
	int maxPacketsPerTick = 3;
	int maxPacketsPerSecond = 14;
	bool boost = false;
	float boostAmount = 1.5f;
	float boostDelay = 8.0f;
	int hitresultFE = 1;

	// State variables
	std::chrono::time_point<std::chrono::steady_clock> lastAttackTime;
	int attackCounter = 0;
	float accumulatedTime = 0.f;
	int packetAmount = 1;
	int aimPosMode = 0;
	int metaRotMode = 0;

	// TRI State
	int packetsThisTick = 0;
	int totalPacketsThisSecond = 0;
	float packetResetTimer = 0.f;
	float lastDeltaTime = 0.016f;
	Vec3<float> targetVelocity;
	Vec3<float> lastTargetPos;
	bool targetAcquired = false;
	float currentYaw = 0.f;
	float currentPitch = 0.f;
	bool isBoosting = false;
	float boostTimer = 0.f;
	float lastBoostTime = 0.f;
	bool attacking12 = false;
	int consecutiveHits = 0;
	float lastHitTime = 0.f;
	int missCount = 0;
	float adaptiveCooldown = 0.f;
	int cachedBestSlot = -1;
	bool slotCacheValid = false;
	int delayOFE = 0;

	// Helpers
	Vec2<float> normAngles(Vec2<float> angle);
	float smoothRotation(float current, float target, float speed, float deltaTime);
	Vec2<float> calculatePredictedAim(LocalPlayer* localPlayer, Actor* target);
	void updateTargetVelocity(Actor* target, float deltaTime);
	bool canSendPacket();
	void resetPacketCounters(float deltaTime);
	void sendExploitPackets(LocalPlayer* localPlayer, Actor* target);

public:
	bool render = false;
	UIColor renderColor = UIColor(255, 255, 255, 255);
	float antiKillauraRange = 0.f;
	bool antiKillaura = false;
	bool rotateBody = false;

private:
	std::vector<Actor*> targetList;
	int oTick = INT_MAX;
	bool shouldRot = false;
	Vec2<float> rotAngle;

	static bool sortByDist(Actor* a1, Actor* a2);
	int getBestWeaponSlot(Actor* target);
	void Attack(Actor* target);

public:
	KillAura2();

	std::string getModeText() override;
	void onDisable() override;
	void onEnable() override;
	void onNormalTick(LocalPlayer* localPlayer) override;
	void onUpdateRotation(LocalPlayer* localPlayer) override;
	void onSendPacket(Packet* packet, bool& cancel) override;
	void onLevelRender() override;
};