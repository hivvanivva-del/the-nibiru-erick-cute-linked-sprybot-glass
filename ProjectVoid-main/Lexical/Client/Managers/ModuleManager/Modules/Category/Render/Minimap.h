#pragma once
#include "../../ModuleBase/Module.h"
#include <vector>
#include <mutex>

struct CachedEntity {
    Vec3<float> pos;
    UIColor color;
};

class Minimap : public Module {
private:
    int radius = 32;
    int zoom = 2;
    int opacity = 180;
    int size = 120;
    bool showEntities = true;

    // Cache to prevent crashing by scanning on render thread
    std::mutex mapMutex;
    std::vector<UIColor> mapCache;
    std::vector<CachedEntity> entityCache;
    int cachedRadius = 0;

public:
    Minimap();

    void onNormalTick(LocalPlayer* lp) override;
    void onD2DRender() override;
};