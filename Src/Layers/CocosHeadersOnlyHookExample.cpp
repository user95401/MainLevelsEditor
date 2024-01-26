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
#include <gd.h_2.1/layers_scenes_transitions_nodes/CreatorLayer.h>
//class for getting functions from that
class CreatorLayerSkit : public CCLayer {
public:
    //gd::CreatorLayer::create()->onChallenge()
    //https://github.com/geode-sdk/bindings/blob/eefa76cafa4c1d637325f5f5866ddd55b9fac64c/bindings/2.200/GeometryDash.bro#L162
    void sus_70330(CCObject* pSender) {//onMultiplayer
        //55 8b ec 83 e4 f8 6a ff 68 88 c2
        uintptr_t addr = patterns::find_pattern("????? f8 6a ff 68 88 c2");
        //addr = gd::base + 0x707f0; //2.204
        //ModUtils::OpenConsole();
        //ModUtils::log((ModUtils::ReadProcMemAsStr(addr, 26)).c_str());
        if (!addr) return;
        reinterpret_cast<void(__thiscall*)(void*, CCObject*)>(addr)(this, pSender);
    }
    void sus_6FE90(CCObject* pSender) {//onMyLevels
        //55 8b ec 83 e4 f8 56 57 e8 e3 11 b 0 8b b0 cc
        uintptr_t addr = patterns::find_pattern("^ ? ? ? ? ? f8 56 57 e8 e3 11 b 0 8b b0 cc");
        //addr = gd::base + 0x70350; //2.204
        //ModUtils::OpenConsole();
        //ModUtils::log((ModUtils::ReadProcMemAsStr(addr, 16)).c_str());
        if (!addr) return;
        reinterpret_cast<void(__thiscall*)(void*, CCObject*)>(addr)(this, pSender);
    }
};

void MenuLayerSkit::onSomeBtn(CCObject* pSender) {
    //asdsed
    FLAlertLayer* alert = FLAlertLayer::create(nullptr, 
        "some text", 
        std::string("Just for example)\n<cr>My lady came down, she was thinking no harm Long Lankin stood ready to catch her in his arm There's blood in the kitchen. There's blood in the hall There's blood in the parlour where my lady did fall You might also like Long Lankin Steeleye Span Immolation of Night Invent Animate Without a Whisper Invent Animate -O master, O master, don't lay the blame on me 'Twas the false nurse and Lankin that killed your lady. Long Lankin was hung on a gibbet so high And the false nurse was burnt in a fire close by</c>"),
        "Oh ok", nullptr);
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
    
    //CCLayerGradient for idk
    CCLayerGradient* pCCLayerGradient = CCLayerGradient::create({ 90,190,255,110 }, { 0,0,0,0 });
    pCCLayerGradient->setBlendFunc({ GL_SRC_ALPHA, GL_ONE });//additive
    self->addChild(pCCLayerGradient, 100);

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
    CCMenuItemLabelExt* btn_chatHistory_001 = CCMenuItemLabelExt::create(
        ModUtils::createSprite("btn_chatHistory_001.png"),
        self,
        menu_selector(MenuLayerSkit::onSomeBtn)
    );
    btn_chatHistory_001->setPositionX(CCDirector::sharedDirector()->getWinSize().width - 42);
    btn_chatHistory_001->setPositionY(48.000f);
    Menu->addChild(btn_chatHistory_001);

    //CreatorLayerSkit::sus_70330
    CCMenuItemLabelExt* versus = CCMenuItemLabelExt::create(
        CCLabelBMFont::create("sus", "gjFont30.fnt"),
        self,
        menu_selector(CreatorLayerSkit::sus_70330)
    );
    versus->setPositionX(CCDirector::sharedDirector()->getWinSize().width - 32);
    versus->setPositionY(88.000f);
    versus->setScale(0.600f);//yo
    Menu->addChild(versus);

    //CustomLayer::pushToMe
    CCMenuItemLabelExt* GJ_everyplayBtn_001 = CCMenuItemLabelExt::create(
        ModUtils::createSprite("GJ_everyplayBtn_001.png"),
        self,
        menu_selector(CustomLayer::pushToMe)
    );
    GJ_everyplayBtn_001->setPositionX(CCDirector::sharedDirector()->getWinSize().width - 32);
    GJ_everyplayBtn_001->setPositionY(128.000f);
    Menu->addChild(GJ_everyplayBtn_001);

    //CreatorLayerSkit::sus_6FE90
    CCMenuItemLabelExt* dialogIcon_052 = CCMenuItemLabelExt::create(
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

    //as i see hooking rewrites bytes up to e4
    //and before functoin must be debuger trap cc sometimes
    
    //asd22:07:58 CE [mod]: oldfncinstruction:  55 8b ec 83 e4 f8 83 ec 74 53 56 8b d9 57 89 5c
    //22:07:58 CE [mod]: 12825680 [hook]: MH_OK
    //22:07:58 CE [mod]: hookedfncinstruction:  e9 73 5b 2d 02 f8 83 ec 74 53 56 8b d9 57 89 5c
    uintptr_t addr = patterns::find_pattern("cc^?  ?  ?  ?  ?  F8 83 EC 74 53 56 8b d9 57 89 5c");
    //addr = gd::base + 0x27b450; //2.204
    //ModUtils::OpenConsole();
    //ModUtils::log(("oldfncinstruction: " + ModUtils::ReadProcMemAsStr(addr, 16)).c_str());
    MappedHooks::registerHook(addr, MenuLayer_init);
    //ModUtils::log(("hookedfncinstruction: " + ModUtils::ReadProcMemAsStr(addr, 16)).c_str());
}