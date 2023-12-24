#include "CocosHeadersOnlyHookExample.hpp"

//and also MappedHooks lol

/*
*  here I'll show you how to write a layer hook without gd.h
*/

//for jump to document in ide and other view triks
#include <gd.h_2.113/layers_scenes_transitions_nodes/CreatorLayer.h>
//class for getting functions from that
class CreatorLayerSkit : public CCLayer {
public:
    //gd::CreatorLayer::create()->onChallenge()
    //https://github.com/geode-sdk/bindings/blob/eefa76cafa4c1d637325f5f5866ddd55b9fac64c/bindings/2.200/GeometryDash.bro#L162
    void sus_70330(CCObject* pSender) {//onMultiplayer
        reinterpret_cast<void(__thiscall*)(void*, CCObject*)>(gd::base + 0x70330)(this, pSender);
    }
    void sus_6FE90(CCObject* pSender) {//onMyLevels
        reinterpret_cast<void(__thiscall*)(void*, CCObject*)>(gd::base + 0x6FE90)(this, pSender);
    }
};

bool __fastcall MenuLayer_init(MenuLayerSkit* self) {
    MappedHooks::getOriginal(MenuLayer_init)(self);
    self->me = self;
    twoTimesBoolCallEscapeByParrentNode(self);//for geode loader

    //snow.
    CCParticleSnow* pCCParticleSnow = CCParticleSnow::create();
    self->addChild(pCCParticleSnow, 101, 2024);

    CCSprite* spr = ModUtils::createSprite("tutorial_01.png");
    spr->setPosition(ModUtils::getCenterPoint());
    self->addChild(spr, 0, 666);

    CCMenu* Menu = CCMenu::create();
    Menu->setPosition(CCPoint());
    self->addChild(Menu, 10, 5940);//do u know that exists addChild(node, index) and addChild(node, index, tag)???

    //CreatorLayerSkit::sus_70330
    //we don't have robtop addons, so take CCMenuItemLabel because it have some animation at least
    CCMenuItemLabel* btn_chatHistory_001 = CCMenuItemLabel::create(
        ModUtils::createSprite("btn_chatHistory_001.png"),
        self,
        menu_selector(CreatorLayerSkit::sus_70330)
    );
    btn_chatHistory_001->setPositionX(CCDirector::sharedDirector()->getWinSize().width - 42);
    btn_chatHistory_001->setPositionY(48.000f);
    Menu->addChild(btn_chatHistory_001);

    //CreatorLayerSkit::sus_6FE90
    //we don't have robtop addons, so take CCMenuItemLabel because it have some animation at least
    CCMenuItemLabel* dialogIcon_052 = CCMenuItemLabel::create(
        ModUtils::createSprite("dialogIcon_052.png"),
        self,
        menu_selector(CreatorLayerSkit::sus_6FE90)
    );
    dialogIcon_052->setPositionX(CCDirector::sharedDirector()->getWinSize().width - 62);
    dialogIcon_052->setPositionY(128.000f);
    Menu->addChild(dialogIcon_052);

    return true;
}

void MenuLayerSkit::CreateHooks() {
    //gd::base it is reinterpret_cast<uintptr_t>(GetModuleHandle(0))
    MappedHooks::registerHook((DWORD)GetModuleHandle(0) + 0x276700, MenuLayer_init);
}