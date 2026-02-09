#include "Minimap.h"
#include "../../../ModuleManager.h"
#include <algorithm>

Minimap::Minimap() : Module("Minimap", "Shows a top-down view of your surroundings", Category::MISC) {
    registerSetting(new SliderSetting<int>("Size", "Size of the map", &size, 120, 50, 300));
    registerSetting(new SliderSetting<int>("Radius", "Area to scan", &radius, 32, 16, 64));
    registerSetting(new SliderSetting<int>("Zoom", "Zoom level", &zoom, 2, 1, 4));
    registerSetting(new SliderSetting<int>("Opacity", "Background opacity", &opacity, 180, 0, 255));
    registerSetting(new BoolSetting("Entities", "Show nearby entities", &showEntities, true));
}

UIColor getBlockColor(const std::string& name) {
    if (name.find("grass") != std::string::npos) return UIColor(100, 150, 60);
    if (name.find("dirt") != std::string::npos) return UIColor(120, 80, 50);
    if (name.find("stone") != std::string::npos) return UIColor(100, 100, 100);
    if (name.find("water") != std::string::npos) return UIColor(40, 80, 200);
    if (name.find("lava") != std::string::npos) return UIColor(255, 100, 0);
    if (name.find("sand") != std::string::npos) return UIColor(220, 220, 150);
    if (name.find("tree") != std::string::npos || name.find("leaves") != std::string::npos || name.find("log") != std::string::npos) return UIColor(40, 100, 30);
    if (name.find("snow") != std::string::npos) return UIColor(240, 240, 240);
    if (name.find("air") != std::string::npos) return UIColor(0, 0, 0, 0);
    if (name.find("deepslate") != std::string::npos) return UIColor(50, 50, 55);
    if (name.find("bedrock") != std::string::npos) return UIColor(30, 30, 30);

    return UIColor(150, 150, 150);
}

void Minimap::onNormalTick(LocalPlayer* lp) {
    if (lp == nullptr || lp->stateVector == nullptr || !Game::clientInstance) return;

    BlockSource* region = Game::clientInstance->getRegion();
    if (region == nullptr) return;

    Vec3<float> myPos = lp->getPos();
    int scanRad = (int)radius;
    int side = scanRad * 2;

    // Use local buffers to avoid holding the lock during expensive scanning
    std::vector<UIColor> localMap;
    localMap.assign((size_t)side * side, UIColor(0, 0, 0, 0));

    int centerX = (int)floorf(myPos.x);
    int centerZ = (int)floorf(myPos.z);
    int playerY = (int)floorf(myPos.y);

    for (int x = -scanRad; x < scanRad; x++) {
        for (int z = -scanRad; z < scanRad; z++) {
            int cacheIdx = (x + scanRad) * side + (z + scanRad);
            BlockPos scanPos(centerX + x, playerY, centerZ + z);

            bool found = false;
            // Scan 50 above and 100 below to ensure no black screens
            for (int yOff = 50; yOff > -100; yOff--) {
                scanPos.y = playerY + yOff;
                Block* block = region->getBlock(scanPos);
                if (block != nullptr && block->blockLegacy != nullptr) {
                    std::string& name = block->blockLegacy->mName;
                    if (name.find("air") == std::string::npos) {
                        UIColor color = getBlockColor(name);

                        // Subtle Height shading
                        if (yOff > 0) {
                            color.r = (unsigned char)(std::min)(255, (int)color.r + yOff * 2);
                            color.g = (unsigned char)(std::min)(255, (int)color.g + yOff * 2);
                            color.b = (unsigned char)(std::min)(255, (int)color.b + yOff * 2);
                        }
                        else if (yOff < 0) {
                            color.r = (unsigned char)(std::max)(0, (int)color.r + yOff * 1);
                            color.g = (unsigned char)(std::max)(0, (int)color.g + yOff * 1);
                            color.b = (unsigned char)(std::max)(0, (int)color.b + yOff * 1);
                        }
                        localMap[cacheIdx] = color;
                        found = true;
                        break;
                    }
                }
            }
        }
    }

    // ... (entities logic same) ...
    std::vector<CachedEntity> localEntities;
    if (showEntities && lp->level) {
        for (auto actor : lp->level->getRuntimeActorList()) {
            if (actor == nullptr || actor == lp || !actor->isAlive() || actor->stateVector == nullptr) continue;

            CachedEntity cent;
            cent.pos = actor->getPos();
            cent.color = UIColor(255, 255, 255);

            ActorTypeComponent* typeComp = actor->getActorTypeComponent();
            if (typeComp && (typeComp->id == 319 || typeComp->id == 307)) {
                cent.color = UIColor(255, 0, 0); // Enemy Player
                std::string* namePtr = actor->getNameTag();
                if (namePtr && FriendManager::isInList(*namePtr)) {
                    cent.color = UIColor(0, 255, 255); // Friend
                }
            }
            else {
                cent.color = UIColor(255, 255, 0); // Mob
            }
            localEntities.push_back(cent);
        }
    }

    // Swap buffers under lock
    {
        std::lock_guard<std::mutex> lock(mapMutex);
        mapCache = std::move(localMap);
        entityCache = std::move(localEntities);
        cachedRadius = scanRad;
    }
}

void Minimap::onD2DRender() {
    LocalPlayer* lp = Game::getLocalPlayer();
    if (lp == nullptr || lp->stateVector == nullptr || !Game::clientInstance) return;

    Vec3<float> myPos = lp->getPos();
    Vec2<float> windowSize = D2D::getWindowSize();

    float mapSize = (float)size;
    float posX = windowSize.x - mapSize - 20.0f;
    float posY = 20.0f;
    Vec4<float> mapRect(posX, posY, posX + mapSize, posY + mapSize);

    // Premium Frame (Multiple layers for depth)
    D2D::fillRectangle(Vec4<float>(posX - 5, posY - 5, posX + mapSize + 5, posY + mapSize + 5), UIColor(10, 10, 10, 255)); // Outer dark
    D2D::fillRectangle(Vec4<float>(posX - 3, posY - 3, posX + mapSize + 3, posY + mapSize + 3), UIColor(45, 45, 45, 255)); // Inner grey
    D2D::drawRectangle(Vec4<float>(posX - 5, posY - 5, posX + mapSize + 5, posY + mapSize + 5), UIColor(0, 0, 0, 255), 1.0f); // Outline
    D2D::drawRectangle(Vec4<float>(posX - 1, posY - 1, posX + mapSize + 1, posY + mapSize + 1), UIColor(0, 0, 0, 255), 1.0f); // Inner outline

    // Background
    D2D::fillRectangle(mapRect, UIColor(20, 20, 20, opacity));

    float pixelSize = (float)zoom;
    int renderRad = (int)(mapSize / (pixelSize * 2.0f));

    // Lock while drawing from cache
    {
        std::lock_guard<std::mutex> lock(mapMutex);
        if (mapCache.empty()) return;

        int scanRad = cachedRadius;
        int side = scanRad * 2;
        float mapCenterX = posX + (mapSize / 2.0f);
        float mapCenterY = posY + (mapSize / 2.0f);

        // Draw Terrain
        for (int x = -renderRad; x < renderRad; x++) {
            for (int z = -renderRad; z < renderRad; z++) {
                if (x < -scanRad || x >= scanRad || z < -scanRad || z >= scanRad) continue;

                int cacheIdx = (x + scanRad) * side + (z + scanRad);
                if (cacheIdx < 0 || cacheIdx >= (int)mapCache.size()) continue;

                UIColor color = mapCache[cacheIdx];
                if (color.a == 0) continue;

                float drawX = mapCenterX + (float)x * pixelSize;
                float drawY = mapCenterY + (float)z * pixelSize;

                if (drawX >= posX && drawX < posX + mapSize && drawY >= posY && drawY < posY + mapSize) {
                    D2D::fillRectangle(Vec4<float>(drawX, drawY, drawX + pixelSize, drawY + pixelSize), color);
                }
            }
        }

        // Draw Entities
        if (showEntities) {
            for (const auto& ent : entityCache) {
                float diffX = ent.pos.x - myPos.x;
                float diffZ = ent.pos.z - myPos.z;

                float drawX = mapCenterX + diffX * pixelSize;
                float drawY = mapCenterY + diffZ * pixelSize;

                if (drawX >= posX && drawX <= posX + mapSize && drawY >= posY && drawY <= posY + mapSize) {
                    D2D::fillCircle(Vec2<float>(drawX, drawY), ent.color, 2.0f);
                }
            }
        }
    }

    // Player Indicator (Middle) - Triangular pointer with outline
    float mapCenterX = posX + (mapSize / 2.0f);
    float mapCenterY = posY + (mapSize / 2.0f);

    if (lp->rotation) {
        float rot = lp->rotation->presentRot.y;
        float rad = (rot + 90.0f) * (3.14159f / 180.0f);

        float pointerSize = 7.0f;
        Vec2<float> p1(mapCenterX + cosf(rad) * pointerSize, mapCenterY + sinf(rad) * pointerSize);
        Vec2<float> p2(mapCenterX + cosf(rad + 2.4f) * pointerSize, mapCenterY + sinf(rad + 2.4f) * pointerSize);
        Vec2<float> p3(mapCenterX + cosf(rad - 2.4f) * pointerSize, mapCenterY + sinf(rad - 2.4f) * pointerSize);

        D2D::fillTriangle(p1, p2, p3, UIColor(255, 255, 255, 255));
        D2D::drawTriangle(p1, p2, p3, UIColor(0, 0, 0, 255), 1.0f);
    }
    else {
        D2D::fillCircle(Vec2<float>(mapCenterX, mapCenterY), UIColor(255, 255, 255), 3.0f);
    }
}