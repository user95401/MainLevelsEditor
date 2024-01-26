#include "PlayLayerExt.hpp"

/// <summary>
/// shit
/// </summary>

class InternalExtensions : public PlayLayerExt {
public:
    void schd1(float) {
        showHint();
    }
    void schd2(float) {
        showNewBest(1,0,0,1,1,1);
    }
};

//0x2d69a0
bool __fastcall PlayLayer_init(PlayLayerExt* self, int a1, int a2, int a3, int a4) {
    MappedHooks::getOriginal(PlayLayer_init)(self, a1, a2, a3, a4);
    twoTimesBoolCallEscapeByParrentNode(self);//for geode loader
    CCMessageBox(std::format("PlayLayer_init({}, {}, {}, {})", a1, a2, a3, a4).c_str(), __FUNCSIG__);

    //snow.
    CCParticleSnow* pCCParticleSnow = CCParticleSnow::create();
    //pCCParticleSnow->setBlendAdditive(true); still no
    pCCParticleSnow->setBlendFunc({ GL_SRC_ALPHA, GL_ONE }/*that is additive blend*/);//but this works :D
    self->addChild(pCCParticleSnow, 101, 2024);

    self->scheduleOnce(schedule_selector(InternalExtensions::schd1), 1.f);
    self->schedule(schedule_selector(InternalExtensions::schd2), 1.f);

    return true;
}

//0x2D5cF0
PlayLayer* __fastcall PlayLayer_create(GJGameLevel* a2, bool a3, bool a4) {
    //auto level = GameLevelManager::sharedState()->getMainLevel(rand() % 12, false);

    return MappedHooks::getOriginal(PlayLayer_create)(a2, a3, a4);
}

void PlayLayerExt::CreateHooks() {
    //MappedHooks::registerHook(base + 0x2d69a0, PlayLayer_init);
    MappedHooks::registerHook(base + 0x2D5cF0, PlayLayer_create);
}