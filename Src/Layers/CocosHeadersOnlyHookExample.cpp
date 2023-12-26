#include "CocosHeadersOnlyHookExample.hpp"
#include "CustomLayer.hpp"

//and also MappedHooks lol

/*
*  here I'll show you how to write a layer hook without gd.h
*/

//if u want, u cant make extension of CCMenuItemLabel to add animations ant other stuff
//i created copy of robtop's CCMenuItemSpriteExtra that have same behavior
//i put it to CCMenuItemLabelExt.cpp so lets include its header
#include "../CCMenuItemLabelExt.hpp"

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

void MenuLayerSkit::onSomeBtn(CCObject* pSender) {
    FLAlertLayer* alert = FLAlertLayer::create(nullptr, "some text", "Oh ok", nullptr, 490.000f, std::string("Just for example)\n<cr>My lady came down, she was thinking no harm Long Lankin stood ready to catch her in his arm There's blood in the kitchen. There's blood in the hall There's blood in the parlour where my lady did fall You might also like Long Lankin Steeleye Span Immolation of Night Invent Animate Without a Whisper Invent Animate -O master, O master, don't lay the blame on me 'Twas the false nurse and Lankin that killed your lady. Long Lankin was hung on a gibbet so high And the false nurse was burnt in a fire close by</c>"));
    alert->getLayer()->runAction(
        CCRepeatForever::create(
            CCSequence::create(
                CCEaseSineInOut::create(CCMoveBy::create(1.000f, CCPoint(0.000f, 3.000f))),
                CCEaseSineInOut::create(CCMoveBy::create(2.000f, CCPoint(0.000f, -3.000f))),
                nullptr
            )
        )
    ); //for fun xdd
    alert->show();
}

bool __fastcall MenuLayer_init(MenuLayerSkit* self) {
    MappedHooks::getOriginal(MenuLayer_init)(self);
    self->me = self;
    twoTimesBoolCallEscapeByParrentNode(self);//for geode loader

    //snow.
    CCParticleSnow* pCCParticleSnow = CCParticleSnow::create();
    //pCCParticleSnow->setBlendAdditive(true); still no
    pCCParticleSnow->setBlendFunc({ GL_SRC_ALPHA, GL_ONE }/*that is additive blend*/);//but this works :D
    self->addChild(pCCParticleSnow, 101, 2024);

    CCSprite* spr = ModUtils::createSprite("tutorial_01.png");
    spr->setPosition(ModUtils::getCenterPoint());
    spr->setOpacity(120);
    spr->setBlendFunc({ GL_SRC_ALPHA, GL_ONE }/*that is additive blend*/);
    self->addChild(spr, 0, 666);

    CCMenu* Menu = CCMenu::create();
    Menu->setPosition(CCPoint());
    self->addChild(Menu, 10, 5940);//do u know that exists addChild(node, index) and addChild(node, index, tag)???

    //MenuLayerSkit::onSomeBtn
    //we don't have robtop addons, so take CCMenuItemLabel because it have some animation at least
    //but we have CCMenuItemLabelExt that have similar anims from gd so use it
    CCMenuItemSpriteExtra* btn_chatHistory_001 = CCMenuItemSpriteExtra::create(
        ModUtils::createSprite("btn_chatHistory_001.png"),
        self,
        menu_selector(MenuLayerSkit::onSomeBtn)
    );
    btn_chatHistory_001->setPositionX(CCDirector::sharedDirector()->getWinSize().width - 42);
    btn_chatHistory_001->setPositionY(48.000f);
    Menu->addChild(btn_chatHistory_001);

    //CreatorLayerSkit::sus_70330
    CCMenuItemSpriteExtra* GJ_everyplayBtn_001 = CCMenuItemSpriteExtra::create(
        ModUtils::createSprite("GJ_everyplayBtn_001.png"),
        self,
        menu_selector(CustomLayer::pushToMe)
    );
    GJ_everyplayBtn_001->setPositionX(CCDirector::sharedDirector()->getWinSize().width - 32);
    GJ_everyplayBtn_001->setPositionY(88.000f);
    Menu->addChild(GJ_everyplayBtn_001);

    //CreatorLayerSkit::sus_6FE90
    CCMenuItemSpriteExtra* dialogIcon_052 = CCMenuItemSpriteExtra::create(
        ModUtils::createSprite("dialogIcon_052.png"),
        self,
        menu_selector(CreatorLayerSkit::sus_6FE90)
    );
    dialogIcon_052->setPositionX(CCDirector::sharedDirector()->getWinSize().width - 110);
    dialogIcon_052->setPositionY(80.000f);
    Menu->addChild(dialogIcon_052);

    return true;
}

void MenuLayerSkit::CreateHooks() {
    //gd::base it is reinterpret_cast<uintptr_t>(GetModuleHandle(0))
    MappedHooks::registerHook((DWORD)GetModuleHandle(0) + 0x276700, MenuLayer_init);
}