#include "KillAura2.h"
#include "../../../ModuleManager.h"
#include "../../../../../../Renderer/MCR.h"
#include "../../../../../../Renderer/D2D.h"
#include <chrono>
#include <algorithm>
#include <cmath>

// Global random device
static std::random_device rd;
static std::mt19937 gen(rd());

KillAura2::KillAura2() : Module("KillAuraX", "Advanced KillAuraNA2 on AuraV6", Category::COMBAT) {
	// Original Settings
	registerSetting(new SliderSetting<float>("Range", "Range in which targets will be hit", &range, 5.f, 3.f, 40.f));
	registerSetting(new SliderSetting<float>("WallRange", "Range in which targets will be hit through walls", &wallRange, 0.f, 0.f, 40.f));
	
	// Rotations
	registerSetting(new BoolSetting("Rotation", "Enable rotation", &rotate, true));
	registerSetting(new SliderSetting<float>("Rot Speed", "Rotation speed", &kiwiSpeed, 2.0f, 0.5f, 4.0f));
	registerSetting(new SliderSetting<float>("Rot Smooth", "Rotation smoothness", &kiwiSmoothness, 0.6f, 0.1f, 1.0f));
	registerSetting(new SliderSetting<float>("Max Angle", "Maximum angle per tick", &kiwiMaxAngle, 180.0f, 30.0f, 360.0f));
	registerSetting(new BoolSetting("Prediction", "Aim prediction", &usePrediction, true));
	registerSetting(new SliderSetting<float>("Predict Amount", "Prediction time", &predictAmount, 0.05f, 0.0f, 0.25f));
	registerSetting(new BoolSetting("Predict Vertical", "Vertical prediction", &predictVertical, true));
	registerSetting(new SliderSetting<float>("Predict Smooth", "Smoothness", &predictSmooth, 0.6f, 0.1f, 0.9f));

	registerSetting(new EnumSetting("AimPos", "Aim at body or eyes", { "Body", "Eyes" }, &aimPosMode, 0));
	registerSetting(new BoolSetting("Body Yaw", "Enable body yaw", &rotateBody, true));
	registerSetting(new BoolSetting("Randomize", "NULL", &randomize, false));
	
	// Combat Settings
	registerSetting(new SliderSetting<int>("CPS", "Clicks per second", &cps, 12, 1, 20));
	registerSetting(new SliderSetting<int>("Hit Chance", "Chance to hit target", &hitChance, 100, 1, 100));
	registerSetting(new BoolSetting("Raytrace", "Verify aim before hitting", &raytrace, false));
	
	registerSetting(new EnumSetting("HitType", "NULL", { "Single", "Multi" }, &hitType, 0));
	registerSetting(new SliderSetting<int>("Hit Attempts", "Attacks per cycle", &hitAttempts, 1, 1, 20)); //changed from 4 to 20
	registerSetting(new SliderSetting<int>("Packets", "Packets per attack", &packetAmount, 1, 1, 4));
	
	registerSetting(new EnumSetting("Weapon", "Auto switch to best weapon", { "None", "Switch", "Spoof" }, &autoWeaponMode, 0));
	registerSetting(new BoolSetting("Mobs", "Attack Mobs", &includeMobs, false));
	registerSetting(new BoolSetting("EatStop", "StopOnEat", &eatStop, false));
	
	// Exploits & Advanced
	registerSetting(new BoolSetting("Reach Exploit", "Extended reach", &useReachExploit, false));
	registerSetting(new SliderSetting<float>("Exploit Reach", "Exploit distance", &exploitReach, 4.0f, 3.5f, 5.0f));
	registerSetting(new SliderSetting<int>("Exploit Packets", "Extra packets", &exploitPackets, 2, 1, 4));
	
	registerSetting(new SliderSetting<int>("Max Packets/Tick", "Packet limit per tick", &maxPacketsPerTick, 3, 1, 6));
	registerSetting(new SliderSetting<int>("Max Packets/Sec", "Packet limit per second", &maxPacketsPerSecond, 14, 8, 25));
	
	registerSetting(new BoolSetting("Boost", "Speed boost", &boost, false));
	registerSetting(new SliderSetting<float>("Boost Amount", "Boost multiplier", &boostAmount, 1.5f, 1.1f, 3.0f));
	registerSetting(new SliderSetting<float>("Boost Delay", "Seconds between boosts", &boostDelay, 8.0f, 3.0f, 30.0f));
	
	registerSetting(new EnumSetting("Hit Result", "Hit result spoof", { "AIR", "ENTITY", "BLOCK", "OUT_OF_RANGE" }, &hitresultFE, 1));
	
	registerSetting(new BoolSetting("CA-Compatibility", "Better compatibility with AutoCrystal", &caCompatibility, false));
	registerSetting(new BoolSetting("Render", "NULL", &render, false));
	registerSetting(new ColorSetting("Color", "Render Color", &renderColor, renderColor, true, true, [&]() -> bool {
		return render;
	}));
	registerSetting(new BoolSetting("Anti Killaura", "Enable Anti Killaura rotation adjustments", &antiKillaura, false));
	registerSetting(new SliderSetting<float>("AntiKA Range", "Range to trigger Anti Killaura predictive rotations", &antiKillauraRange, 2.f, 0.f, 5.f));
}

std::string KillAura2::getModeText() {
	switch (hitType) {
	case 0: return "Single";
	case 1: return "Multi";
	}
	return "NULL";
}

// Helper Functions
Vec2<float> KillAura2::normAngles(Vec2<float> angle) {
	while (angle.y > 180.0f) angle.y -= 360.0f;
	while (angle.y < -180.0f) angle.y += 360.0f;
	if (angle.x > 89.0f) angle.x = 89.0f;
	if (angle.x < -89.0f) angle.x = -89.0f;
	return angle;
}

bool KillAura2::canSendPacket() {
	if (packetsThisTick >= maxPacketsPerTick) return false;
	if (totalPacketsThisSecond >= maxPacketsPerSecond) return false;
	return true;
}

void KillAura2::resetPacketCounters(float deltaTime) {
	packetsThisTick = 0;
	packetResetTimer += deltaTime;
	if (packetResetTimer >= 1.0f) {
		totalPacketsThisSecond = 0;
		packetResetTimer = 0.f;
	}
}

float KillAura2::smoothRotation(float current, float target, float speed, float deltaTime) {
	float diff = target - current;
	while (diff <= -180.0f) diff += 360.0f;
	while (diff > 180.0f) diff -= 360.0f;

	// Improved smoothing:
	// 1. Exponential smoothing for main movement (organic feel)
	// 2. Minimum velocity to prevent "Zeno's paradox" lag at end
	// 3. Snap when very close
	
	if (std::abs(diff) < 0.5f) return target; // Snap if very close to prevent jitter

	float t = std::clamp(speed * deltaTime * 5.0f, 0.05f, 1.0f);
	float move = diff * t;

	// Ensure minimum movement if far away, to catch up
	float minMove = speed * deltaTime * 10.0f; // Linear component
	if (std::abs(move) < minMove && std::abs(diff) > minMove) {
		move = (diff > 0) ? minMove : -minMove;
	}

	float result = current + move;

	while (result > 180.f) result -= 360.f;
	while (result < -180.f) result += 360.f;

	return result;
}

int KillAura2::getBestWeaponSlot(Actor* target) {
	LocalPlayer* localPlayer = Game::getLocalPlayer();
	if (!localPlayer || !target) return localPlayer->playerInventory->selectedSlot;

	if (slotCacheValid && cachedBestSlot >= 0) {
		return cachedBestSlot;
	}

	float bestDamage = 0.f;
	int bestSlot = localPlayer->playerInventory->selectedSlot;
	int originalSlot = localPlayer->playerInventory->selectedSlot;

	for (int i = 0; i < 9; i++) {
		localPlayer->playerInventory->selectedSlot = i;
		ItemStack* item = localPlayer->getCarriedItem();
		// if (!item || !item->isValid()) continue; // Allow hands

		float damage = localPlayer->calculateAttackDamage(target);
		if (damage > bestDamage) {
			bestDamage = damage;
			bestSlot = i;
		}
	}

	localPlayer->playerInventory->selectedSlot = originalSlot;
	cachedBestSlot = bestSlot;
	slotCacheValid = true;

	return bestSlot;
}

Vec2<float> KillAura2::calculatePredictedAim(LocalPlayer* localPlayer, Actor* target) {
	if (!localPlayer || !target) return Vec2<float>(0, 0);

	Vec3<float> targetPos = target->getEyePos();
	if (aimPosMode == 0) { // Body
		targetPos.y = target->aabbShape->aabb.getCenter().y;
	}

	if (usePrediction && targetAcquired) {
		float clampedPredict = std::min(predictAmount, 0.25f);

		Vec3<float> predictedOffset;
		predictedOffset.x = targetVelocity.x * clampedPredict;
		predictedOffset.z = targetVelocity.z * clampedPredict;

		float maxOffset = 0.4f;
		predictedOffset.x = std::clamp(predictedOffset.x, -maxOffset, maxOffset);
		predictedOffset.z = std::clamp(predictedOffset.z, -maxOffset, maxOffset);

		targetPos.x += predictedOffset.x;
		targetPos.z += predictedOffset.z;

		if (predictVertical) {
			float vertOffset = targetVelocity.y * clampedPredict * 0.3f;
			vertOffset = std::clamp(vertOffset, -0.2f, 0.2f);
			targetPos.y += vertOffset;
		}
	}

	Vec3<float> delta = targetPos.sub(localPlayer->getEyePos());

	float yaw = atan2(delta.z, delta.x) * (180.0f / 3.14159265358979323846f) - 90.0f;
	float horizontalDistance = sqrt(delta.x * delta.x + delta.z * delta.z);
	float pitch = -atan2(delta.y, horizontalDistance) * (180.0f / 3.14159265358979323846f);

	pitch = std::clamp(pitch, -89.0f, 89.0f);

	if (randomize) {
		std::uniform_real_distribution<float> jitter(-0.5f, 0.5f);
		yaw += jitter(gen);
		pitch += jitter(gen) * 0.3f;
	}

	return Vec2<float>(pitch, yaw);
}

void KillAura2::updateTargetVelocity(Actor* target, float deltaTime) {
	if (!target) return;

	Vec3<float> currentPos = target->getPos();

	if (!targetAcquired) {
		lastTargetPos = currentPos;
		targetVelocity = Vec3<float>(0, 0, 0);
		targetAcquired = true;
		return;
	}

	Vec3<float> rawVelocity = currentPos.sub(lastTargetPos);

	if (deltaTime > 0.001f) {
		rawVelocity.x /= deltaTime;
		rawVelocity.y /= deltaTime;
		rawVelocity.z /= deltaTime;
	}

	float smooth = std::clamp(predictSmooth, 0.1f, 0.9f);
	// Fixed velocity interpolation (removed incorrect deltaTime multiplication)
	targetVelocity.x = targetVelocity.x * (1.0f - smooth) + rawVelocity.x * smooth;
	targetVelocity.y = targetVelocity.y * (1.0f - smooth) + rawVelocity.y * smooth;
	targetVelocity.z = targetVelocity.z * (1.0f - smooth) + rawVelocity.z * smooth;

	lastTargetPos = currentPos;
}

void KillAura2::sendExploitPackets(LocalPlayer* localPlayer, Actor* target) {
	if (!localPlayer || !target || !useReachExploit) return;
	if (!canSendPacket()) return;

	int packetsToSend = std::min(exploitPackets, maxPacketsPerTick - packetsThisTick);
	packetsToSend = std::min(packetsToSend, maxPacketsPerSecond - totalPacketsThisSecond);
	packetsToSend = std::min(packetsToSend, 2);

	for (int i = 0; i < packetsToSend; i++) {
		localPlayer->getgameMode()->attack(target);
		packetsThisTick++;
		totalPacketsThisSecond++;
	}
}

// Module Methods
void KillAura2::onDisable() {
	targetList.clear();
	shouldRot = false;
	attackCounter = 0;
	accumulatedTime = 0.f;
	
	delayOFE = 0;
	currentYaw = 0.f;
	currentPitch = 0.f;
	isBoosting = false;
	boostTimer = 0.f;
	attacking12 = false;
	consecutiveHits = 0;
	lastHitTime = 0.f;
	targetAcquired = false;
	packetsThisTick = 0;
	totalPacketsThisSecond = 0;
	slotCacheValid = false;
	cachedBestSlot = -1;
	missCount = 0;
	adaptiveCooldown = 0.f;
}

void KillAura2::onEnable() {
	attackCounter = 0;
	accumulatedTime = 0.f;
	lastAttackTime = std::chrono::steady_clock::now();
	
	delayOFE = 0;
	currentYaw = 0.f;
	currentPitch = 0.f;
	isBoosting = false;
	boostTimer = 0.f;
	lastBoostTime = 0.f;
	attacking12 = false;
	targetList.clear();
	consecutiveHits = 0;
	lastHitTime = 0.f;
	targetAcquired = false;
	packetsThisTick = 0;
	totalPacketsThisSecond = 0;
	packetResetTimer = 0.f;
	slotCacheValid = false;
	cachedBestSlot = -1;
	missCount = 0;
	adaptiveCooldown = 0.f;
}

bool KillAura2::sortByDist(Actor* a1, Actor* a2) {
	Vec3<float> lpPos = Game::getLocalPlayer()->getPos();
	return ((a1->getPos().dist(lpPos)) < (a2->getPos().dist(lpPos)));
}

// Unused legacy attack method, keeping it private/empty or redirecting logic
void KillAura2::Attack(Actor* target) {
	// Logic moved to onNormalTick for better packet control
}

void KillAura2::onNormalTick(LocalPlayer* localPlayer) {
	if (!localPlayer || !localPlayer->gameMode) return;

	BlockSource* region = Game::clientInstance->getRegion();
	Level* level = localPlayer->level;
	if (!level || !region) return;

	static AutoCrystal* crystalAura = ModuleManager::getModule<AutoCrystal>();
	if (crystalAura->isEnabled() && !crystalAura->placeList.empty() && caCompatibility) return;

	float deltaTime = D2D::deltaTime;
	lastDeltaTime = deltaTime;
	lastHitTime += deltaTime;

	resetPacketCounters(deltaTime);

	if (adaptiveCooldown > 0.f) {
		adaptiveCooldown -= deltaTime;
		return;
	}

	if (eatStop && localPlayer->getItemUseDuration() > 0) {
		shouldRot = false;
		return;
	}

	if (boost) {
		if (!isBoosting) {
			lastBoostTime += deltaTime;
			if (lastBoostTime >= boostDelay) {
				isBoosting = true;
				boostTimer = 0.8f;
				lastBoostTime = 0.f;
			}
		}
		else {
			boostTimer -= deltaTime;
			if (boostTimer <= 0.f) {
				isBoosting = false;
			}
		}
	}

	if (cps <= 0) return;

	auto& inventory = *localPlayer->playerInventory;

	float boostMult = isBoosting ? boostAmount : 1.0f;
	// cps conversion to ticks (approx)
	float ticksPerClick = 20.f / (cps * boostMult);
	int attackInterval = std::max(1, (int)ticksPerClick);

	targetList.clear();
	float effectiveRange = useReachExploit ? std::max(range, exploitReach) : range;

	Actor* previousTarget = nullptr;
	if (!targetList.empty()) {
		previousTarget = targetList[0];
	}

	for (Actor* ent : level->getRuntimeActorList()) {
		if (!ent) continue;
		if (!TargetUtil::isTargetValid(ent, includeMobs)) continue;

		float rangeCheck = effectiveRange;
		if (region->getSeenPercent(localPlayer->getEyePos(), ent->aabbShape->aabb) == 0.0f)
			rangeCheck = wallRange;

		float distance = localPlayer->getPos().dist(ent->getPos());
		if (distance <= rangeCheck) {
			targetList.push_back(ent);
		}
	}

	std::sort(targetList.begin(), targetList.end(), KillAura2::sortByDist);

	if (targetList.empty()) {
		shouldRot = false;
		targetAcquired = false;
		consecutiveHits = 0;
		slotCacheValid = false;
		return;
	}

	Actor* primaryTarget = targetList[0];

	if (previousTarget != primaryTarget) {
		targetAcquired = false;
		slotCacheValid = false;
	}

	updateTargetVelocity(primaryTarget, deltaTime);

	if (delayOFE++ >= attackInterval) {
		delayOFE = 0;

		if (!canSendPacket()) {
			return;
		}

		const int oldSlot = inventory.selectedSlot;

		if (autoWeaponMode != 0) {
			int bestSlot = getBestWeaponSlot(primaryTarget);
			if (autoWeaponMode == 1) { // Switch
				inventory.selectedSlot = bestSlot;
			}
			else if (autoWeaponMode == 2) { // Spoof
				// Server side slot spoofing if needed
				if (PlayerUtil::selectedSlotServerSide != bestSlot) {
					MobEquipmentPacket mep(localPlayer->getRuntimeIDComponent()->runtimeId, localPlayer->playerInventory->container->getItem(bestSlot), bestSlot, bestSlot);
					level->getPacketSender()->send(&mep);
				}
			}
		}

		int attacks = std::min(hitAttempts, 3);
		int packets = std::min(packetAmount, 3);

		int totalPacketsNeeded = attacks * packets;
		if (useReachExploit) {
			totalPacketsNeeded += exploitPackets;
		}

		int availablePackets = std::min(maxPacketsPerTick - packetsThisTick,
			maxPacketsPerSecond - totalPacketsThisSecond);

		if (totalPacketsNeeded > availablePackets) {
			float ratio = (float)availablePackets / totalPacketsNeeded;
			attacks = std::max(1, (int)(attacks * ratio));
			packets = std::max(1, (int)(packets * ratio));
		}

		for (int i = 0; i < attacks && canSendPacket(); i++) {
			if (rotate) {
				Vec2<float> targetAim = calculatePredictedAim(localPlayer, primaryTarget);

				currentYaw = smoothRotation(currentYaw, targetAim.y,
					kiwiSpeed * kiwiSmoothness, deltaTime);
				currentPitch = smoothRotation(currentPitch, targetAim.x,
					kiwiSpeed * kiwiSmoothness * 1.5f, deltaTime);
				
				shouldRot = true;
				rotAngle.x = currentPitch;
				rotAngle.y = currentYaw;

				auto& rotation = localPlayer->rotation->presentRot;
				rotation.x = currentPitch;
				rotation.y = currentYaw;

				if (auto* headRot = localPlayer->getActorHeadRotationComponent()) {
					headRot->headYaw = currentYaw;
				}
			}

			for (int p = 0; p < packets && canSendPacket(); p++) {
				// Hit Chance Logic
				if (hitChance < 100) {
					std::uniform_int_distribution<int> chance(0, 100);
					if (chance(gen) > hitChance) {
						// Miss deliberately
						if (p == 0) localPlayer->swing(); // Swing even on miss
						continue; 
					}
				}

				// Raytrace Logic
				if (raytrace && rotate) {
					// Simple angle check
					// If the angle difference between current aim and target is too large, skip hit
					// This simulates looking at the target
					Vec2<float> aim = calculatePredictedAim(localPlayer, primaryTarget);
					float yawDiff = std::abs(aim.y - currentYaw);
					float pitchDiff = std::abs(aim.x - currentPitch);
					
					// Normalize yaw diff
					while (yawDiff > 180.f) yawDiff -= 360.f;
					yawDiff = std::abs(yawDiff);
					
					// Allow some margin of error (e.g., 5-10 degrees depending on distance/hitbox)
					// At 3 blocks, player width ~0.6 -> angle ~ atan(0.3/3) * 2 ~= 11 degrees
					if (yawDiff > 15.0f || pitchDiff > 20.0f) { // Generous but strict enough to avoid "backwards" hits
						// Try to attack, but maybe it won't hit if we rely on server side checks.
						// If we want to strictly prevent sending packet:
						continue;
					}
				}

				localPlayer->getgameMode()->attack(primaryTarget);
				packetsThisTick++;
				totalPacketsThisSecond++;
			}

			if (i == 0 && useReachExploit) {
				sendExploitPackets(localPlayer, primaryTarget);
			}

			localPlayer->swing();
		}

		if (HitResult* hitResult = localPlayer->level->getHitResult()) {
			switch (hitresultFE) {
			case 0: hitResult->type = HitResultType::AIR; break;
			case 1: hitResult->type = HitResultType::ENTITY; break;
			case 2: hitResult->type = HitResultType::BLOCK; break;
			case 3: hitResult->type = HitResultType::ENTITY_OUT_OF_RANGE; break;
			}
		}

		attacking12 = true;
		consecutiveHits++;
		lastHitTime = 0.f;
		missCount = 0;

		if (hitType == 1 && targetList.size() > 1 && canSendPacket()) { // Multi
			Actor* secondTarget = targetList[1];
			if (canSendPacket()) {
				localPlayer->getgameMode()->attack(secondTarget);
				packetsThisTick++;
				totalPacketsThisSecond++;
				localPlayer->swing();
			}
		}

		if (autoWeaponMode == 2) {
			if (inventory.selectedSlot != oldSlot) inventory.selectedSlot = oldSlot;
		}
	}
	else {
		attacking12 = false;

		if (lastHitTime > 1.0f) {
			missCount++;
			if (missCount > 5) {
				adaptiveCooldown = 0.5f;
				missCount = 0;
			}
			consecutiveHits = 0;
		}
	}
}

void KillAura2::onUpdateRotation(LocalPlayer* localPlayer) {
	if (!rotate || targetList.empty()) return;
	if (!localPlayer || !localPlayer->gameMode) return;

	BlockSource* region = Game::clientInstance->getRegion();
	Level* level = localPlayer->level;
	if (!level || !region) return;

	Actor* target = targetList[0];

	Vec2<float> targetAim = calculatePredictedAim(localPlayer, target);

	currentYaw = smoothRotation(currentYaw, targetAim.y,
		kiwiSpeed * kiwiSmoothness, lastDeltaTime);
	currentPitch = smoothRotation(currentPitch, targetAim.x,
		kiwiSpeed * kiwiSmoothness * 1.5f, lastDeltaTime);

	rotAngle.x = currentPitch;
	rotAngle.y = currentYaw;
	shouldRot = true;

	auto& rotation = localPlayer->rotation->presentRot;
	rotation.x = currentPitch;
	rotation.y = currentYaw;

	if (auto* headRot = localPlayer->getActorHeadRotationComponent()) {
		headRot->headYaw = currentYaw;
	}

	if (rotateBody) {
		if (auto* bodyRot = localPlayer->getMobBodyRotationComponent()) {
			bodyRot->bodyYaw = currentYaw;
		}
	}
}

void KillAura2::onSendPacket(Packet* packet, bool& cancel) {
	auto localPlayer = Game::getLocalPlayer();
	if (!localPlayer || !localPlayer->gameMode) return;

	if (packet->getId() == PacketID::PlayerAuthInput) {
		if (!rotate || targetList.empty()) return;

		PlayerAuthInputPacket* paip = static_cast<PlayerAuthInputPacket*>(packet);

		paip->rotation.x = rotAngle.x;
		paip->rotation.y = rotAngle.y;
		paip->headYaw = rotAngle.y;
	}
}

void KillAura2::onLevelRender() {
	if (!render || targetList.empty()) return;

	auto renderTarget = targetList[0];
	if (!renderTarget) return;

	Vec3<float> pos = renderTarget->getPos();
	Vec3<float> posOld = lastTargetPos; // Use last known position instead of getPosOld
	Vec3<float> drawPos = posOld.lerp(pos, D2D::deltaTime, D2D::deltaTime, D2D::deltaTime); // Use D2D::deltaTime
	
	// Re-implementing the visual ring/cylinder
	int quatily = 2; // quality
	float range = 0.8f; // radius

	std::vector<Vec3<float>> posList;

	for (int i = 0; i < 360; i += 5) {
		float rad = (float)i * 3.14159f / 180.f;
		float x = cos(rad) * range;
		float z = sin(rad) * range;
		posList.push_back(drawPos.add(x, 0.f, z));
	}

	if (posList.empty()) return;

	MCR::setColor(renderColor);
	MCR::tessellator->begin(VertextFormat::TRIANGLE_STRIP);

	float yAdd = 0.f;
	int times = 0;
	
	while (true) {
		float oldY = yAdd;
		yAdd += 0.05f; 

		for (size_t i = 0; i < posList.size(); i++) {
			Vec3<float> p1 = posList[i];
			Vec3<float> p2 = posList[(i + 1) % posList.size()];
			
			p1.y += oldY;
			p2.y += oldY;
			
			Vec3<float> p3 = p1; p3.y += 0.05f;
			Vec3<float> p4 = p2; p4.y += 0.05f;

			MCR::tessellator->vertex(p4.x, p4.y, p4.z);
			MCR::tessellator->vertex(p3.x, p3.y, p3.z);
			MCR::tessellator->vertex(p2.x, p2.y, p2.z);
			MCR::tessellator->vertex(p1.x, p1.y, p1.z);
		}

		if (times >= quatily) break;
		times++;
	}

	MeshHelpers::renderMeshImmediately(MCR::screenCtx, MCR::tessellator, MCR::blendMaterial);

	MCR::setColor(renderColor);
	MCR::tessellator->begin(VertextFormat::LINE_LIST);

	for (size_t i = 0; i < posList.size(); i++) {
		Vec3<float> p1 = posList[i];
		Vec3<float> p2 = posList[(i + 1) % posList.size()];

		MCR::tessellator->vertex(p1.x, p1.y, p1.z);
		MCR::tessellator->vertex(p2.x, p2.y, p2.z);
	}

	MeshHelpers::renderMeshImmediately(MCR::screenCtx, MCR::tessellator, MCR::blendMaterial);
}
