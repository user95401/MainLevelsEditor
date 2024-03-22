#include <Geode/Geode.hpp>//Include the Geode headers.
#include <Geode/utils/web.hpp>//Include the Geode headers.
using namespace geode::prelude;//cocos2d and all Geode namespaces

#include "SimpleIni.h"
#include "Header1.hpp"

#ifndef GEODE_IS_WINDOWS
#define GetKeyState(asd) (short)asd
#endif

//funny layers
class MainLevelsEditorLayer : public CCLayer {
public:
    static auto create() {
        auto __this = new MainLevelsEditorLayer;
        if (!__this->init()) CC_SAFE_DELETE(__this);
        //setup
        __this->setKeypadEnabled(true);
        __this->setTouchEnabled(true);
        //BaseContent
        {
            //bg
            CCSprite* backgroundSprite = CCSprite::create("GJ_gradientBG.png");
            backgroundSprite->setScaleX(CCDirector::sharedDirector()->getWinSize().width / backgroundSprite->getContentSize().width);
            backgroundSprite->setScaleY(CCDirector::sharedDirector()->getWinSize().height / backgroundSprite->getContentSize().height);
            backgroundSprite->setAnchorPoint({ 0, 0 });
            backgroundSprite->setColor({ 70, 80, 90 });
            __this->addChild(backgroundSprite, -2);
            //gauntletCorner_001
            CCSprite* gauntletCorner_001 = CCSprite::createWithSpriteFrameName("gauntletCorner_001.png");
            gauntletCorner_001->setPosition({ 0, 0 });
            gauntletCorner_001->setRotation(0);
            gauntletCorner_001->setAnchorPoint({ 0.f,0.f });
            __this->addChild(gauntletCorner_001);//add gauntletCorner_001
            //gauntletCorner_002
            CCSprite* gauntletCorner_002 = CCSprite::createWithSpriteFrameName("gauntletCorner_001.png");
            gauntletCorner_002->setPosition({
                CCDirector::sharedDirector()->getWinSize().width,
                0.f
                });
            gauntletCorner_002->setScaleX(-1.f);
            gauntletCorner_002->setAnchorPoint({ 0.f,0.f });
            __this->addChild(gauntletCorner_002);//add gauntletCorner_002
            //gauntletCorner_003
            CCSprite* gauntletCorner_003 = CCSprite::createWithSpriteFrameName("gauntletCorner_001.png");
            gauntletCorner_003->setPosition({
                (CCDirector::sharedDirector()->getWinSize().width),
                (CCDirector::sharedDirector()->getWinSize().height)
                });
            gauntletCorner_003->setRotation(180.f);
            gauntletCorner_003->setAnchorPoint({ 0.f,0.f });
            __this->addChild(gauntletCorner_003);//add gauntletCorner_003
            //backBtn
            auto menuBack = CCMenu::createWithItem(
                CCMenuItemSpriteExtra::create(CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png"), __this, menu_selector(MainLevelsEditorLayer::onBack))
            );
            menuBack->setPosition({ 25, CCDirector::sharedDirector()->getWinSize().height - 25 });
            __this->addChild(menuBack);
        };
        return __this;
    }
    void openMe(CCObject*) {
        auto scene = CCScene::create();
        auto me = create();
        scene->addChild(me, 0, 3228);
        CCDirector::sharedDirector()->pushScene(CCTransitionFade::create(0.5f, scene));
    }
    void onBack(CCObject* object) {
        keyBackClicked();
    }
    void keyBackClicked() {
        GameManager::sharedState()->fadeInMenuMusic();
        CCDirector::sharedDirector()->popSceneWithTransition(0.5f, PopTransition::kPopTransitionFade);
    }
};

#include <Geode/modify/LevelInfoLayer.hpp>
class $modify(LevelInfoLayerExt, LevelInfoLayer) {
    void SomeSch(float) {
        if (!this) return;
    }
    bool init(GJGameLevel* p0, bool p1) {
        if (Mod::get()->getSettingValue<bool>("COLH"))
            this->schedule(schedule_selector(LevelInfoLayerExt::SomeSch), 0.01f);
        if (Mod::get()->getSettingValue<bool>("SL"))
            p0 = processOutLevelByConfig(p0->m_levelID.value(), p0);
        LevelInfoLayer::init(p0, p1);
        if (Mod::get()->getSettingValue<bool>("UI")) {
            //GJ_copyStateBtn_001
            auto GJ_copyStateBtn_001 = CCMenuItemSpriteExtra::create(
                CCSprite::createWithSpriteFrameName("GJ_copyStateBtn_001.png"),
                this,
                nullptr//menu_selector(LevelInfoLayerExt::OpenGrabLevelPopup1)
            );
            //pCCMenu
            {
                auto pCCMenu = CCMenu::createWithItem(GJ_copyStateBtn_001);
                pCCMenu->setAnchorPoint(CCSizeZero);
                pCCMenu->setScale(.8f);
                pCCMenu->setPositionY(18.f);
                pCCMenu->setPositionX(CCDirector::get()->getScreenRight() - 110);
                pCCMenu->setID("main levels editor ui");
                this->addChild(pCCMenu, 999, 6282);
            };
        }
        return 1;
	}
    TodoReturn setupLevelInfo() {
        if (Mod::get()->getSettingValue<bool>("SL"))
            this->m_level = processOutLevelByConfig(this->m_level->m_levelID.value(), this->m_level);
        LevelInfoLayer::setupLevelInfo();
    };
};

#include <Geode/modify/EditLevelLayer.hpp>
class $modify(EditLevelLayerExt, EditLevelLayer) {
    bool init(GJGameLevel * p0) {
        auto rtn = EditLevelLayer::init(p0);
        if (Mod::get()->getSettingValue<bool>("UI")) {
            //GJ_copyStateBtn_001
            auto GJ_copyStateBtn_001 = CCMenuItemSpriteExtra::create(
                CCSprite::createWithSpriteFrameName("GJ_copyStateBtn_001.png"),
                this,
                menu_selector(MainLevelsEditorLayer::openMe)
            );
            //pCCMenu
            {
                auto pCCMenu = CCMenu::createWithItem(GJ_copyStateBtn_001);
                pCCMenu->setAnchorPoint(CCSizeZero);
                pCCMenu->setScale(.8f);
                pCCMenu->setPositionY(18.f);
                pCCMenu->setPositionX(CCDirector::get()->getScreenRight() - 85);
                pCCMenu->setID("main levels editor ui");
                this->addChild(pCCMenu, 999, 6282);
            };
        }
        return rtn;
    };
};

#include <Geode/modify/LevelSelectLayer.hpp>
class $modify(LevelSelectLayerExt, LevelSelectLayer) {
    void SomeSch(float asd) {
        
    }
    static LevelSelectLayer* create(int p0) {
        auto pLevelSelectLayer = LevelSelectLayer::create(p0);
        if (Mod::get()->getSettingValue<bool>("DMLH")) {
            if (Mod::get()->getSettingValue<bool>("UI")) {
                //GJ_copyStateBtn_001
                auto GJ_copyStateBtn_001 = CCMenuItemSpriteExtra::create(
                    CCSprite::createWithSpriteFrameName("GJ_copyStateBtn_001.png"),
                    pLevelSelectLayer,
                    menu_selector(MainLevelsEditorLayer::openMe)
                );
                //pCCMenu
                {
                    auto pCCMenu = CCMenu::createWithItem(GJ_copyStateBtn_001);
                    pCCMenu->setAnchorPoint(CCSizeZero);
                    pCCMenu->setScale(.8f);
                    pCCMenu->setPositionY(18.f);
                    pCCMenu->setPositionX(CCDirector::get()->getScreenRight() - 85);
                    pCCMenu->setID("main levels editor ui");
                    pLevelSelectLayer->addChild(pCCMenu, 999, 6282);
                };
            }
        };
        return pLevelSelectLayer;
    };
};
/**/