#pragma once
#include "../../ModuleBase/Module.h"

class BedAura : public Module {
public:
	struct Fade {
	public:
		float deltaTime;
		Vec3<float> fadePos;
		float duration;
		Fade(const Vec3<float>& pos, const float& _dur) {
			duration = _dur;
			fadePos = pos;
			deltaTime = 0.f;
		}
	};
	class Bed {
	public:
		float targetDamage;
		float localDamage;
		Actor* actor;
	protected:
		static inline const float computeExplosionDamage(const Vec3<float>& position, Actor* actor, const float& exposure) {
			static constexpr float explosionSize = 12.f;
			const AABB& entityAABB = actor->aabbShape->aabb;
			const Vec3<float>& center = entityAABB.getCenter();
			const Vec3<float>& feetPos = Vec3<float>(center.x, entityAABB.lower.y, center.z);
			const float distance = feetPos.dist(position) / explosionSize;
			if (distance <= 1.f) {
				const float impact = (1.f - distance) * exposure;
				const int damage = std::max((int)(((impact * impact + impact) / 2) * 8 * explosionSize + 1), 0);
				int armorPoints = 0;
				int epf = 0;
				for (int i = 0; i < 4; i++) {
					const ItemStack* armor = actor->getArmor(i);
					if (armor->isValid()) {
						armorPoints += armor->item.get()->getArmorValue();
						epf += 5;
					}
				}
				float finalDamage = damage;
				finalDamage -= finalDamage * armorPoints * 0.04f;
				finalDamage -= finalDamage * std::min(std::ceilf(std::min(epf, 25) * 0.5f), 20.f) * 0.04f;
				return std::max(0.f, finalDamage);
			}
			return 0.0f;
		}
	};

	class Place : public Bed {
	public:
		BlockPos placePos;
		Place(LocalPlayer* localPlayer, const BlockPos& _placePos, Actor* _actor, const float& exposure) {
			placePos = _placePos;
			targetDamage = computeExplosionDamage(_placePos.CastTo<float>(), _actor, exposure);
			localDamage = computeExplosionDamage(_placePos.CastTo<float>(), localPlayer, exposure);
			actor = _actor;
		}
	};

	class Terrain : public Bed {
	public:
		BlockPos placePos;
		BlockPos headPos;
		Terrain(const BlockPos& _blockPos, const BlockPos& _headPos, Actor* _actor, const float& exposure) {
			this->placePos = _blockPos;
			this->headPos = _headPos;
			this->targetDamage = computeExplosionDamage(_blockPos.CastTo<float>().add(0.5f, 1.f, 0.5f), _actor, exposure);
		}
	};
public:
	std::vector<Terrain> terrainList;
	std::vector<Place> placeList;
	std::vector<Actor*> targetList;
	std::vector<BlockPos> extrapolateRender;
	int realYaw = 0;
	int renderYaw = 0;
	bool placeCheck = false;
private:
	// Place
	int placeRange = 6;
	int proximity = 4;
	int placeDelay = 0;
	// Break
	int breakRange = 8;
	int breakDelay = 0;
	// Logic
	int priority = 0;
	bool phasePlace = false;
	bool containerCheck = true;
	bool swap = true;
	bool swapBack = true;
	bool eatStop = true;
	int targetRange = 12;
	float localDamage = 12.f;
	float targetDamage = 4.f;
	bool extrapolation = false;
	int extrapolateAmt = 4;
	bool selfTest = false;
	// Terrain
	bool terrain = false;
	bool packet = false;
	float minTerrain = 8.50f;
	int terrainProximity = 6;
	int terrainDelay = 1;
	// Safety
	bool safety = false;
	float dmgDifference = 3.f;
	float dangerDamage = 8.f;
	// Render
	bool slide = false;
	bool dmgText = false;
	float slideSpeed = 10.f;
	bool fade = false;
	float fadeDuration = 10.f;
	int renderType = 0;
	UIColor damageColor = UIColor(255, 255, 255);
	UIColor lineColor = UIColor(255, 255, 255, 60);
	UIColor renderColor = UIColor(255, 255, 255, 60);
protected:
	float getExposure(const BlockPos& blockPos, Actor* actor);
	static bool sortByDamage(Bed b1, Bed b2) {
		return b1.targetDamage > b2.targetDamage;
	}
	bool isBlockValid(const BlockPos& blockPos, Actor* actor);
	bool setYaw(const BlockPos& bedPos, const BlockPos& headPos) {
		BlockPos pos = BlockPos(headPos.x - bedPos.x, headPos.y - bedPos.y, headPos.z - bedPos.z);
		if (pos == BlockPos(1, 0, 0)) realYaw = -90;
		else if (pos == BlockPos(-1, 0, 0)) realYaw = 90;
		else if (pos == BlockPos(0, 0, 1)) realYaw = 0;
		else if (pos == BlockPos(0, 0, -1)) realYaw = 180;
		return true;
	}
	BlockPos getHeadPos(const BlockPos& bedPos, Actor* actor);
	void generatePlacement(LocalPlayer* localPlayer, Actor* actor);
	void sortPlacement(LocalPlayer* localPlayer, Actor* actor);
	void placeBed(const BlockPos& blockPos);
	void explodeBed(const BlockPos& blockPos);

	BlockPos getHeadPosTerrain(const BlockPos& bedPos, Actor* actor);
	bool isTerrainValid(const BlockPos& blockPos, Actor* actor);
	void placeObsidian(GameMode* gm);
	void getTerrainList(Actor* actor);
	void onTerrain(LocalPlayer* localPlayer, Actor* actor);
public:
	BedAura();
	virtual void onSendPacket(Packet* packet, bool& cancel) override;
	virtual void onNormalTick(LocalPlayer* localPlayer) override;
	virtual void onMCRender(MinecraftUIRenderContext* renderCtx) override;
	virtual void onLevelRender() override;
};