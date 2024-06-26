#include <Geode/Geode.hpp>
using namespace geode::prelude;

#include "Header1.hpp"

#include <Geode/modify/LevelSelectLayer.hpp>
class $modify(LevelSelectLayerExt, LevelSelectLayer) {
    BoomScrollLayer* m_LevelsScrollLayer;
    CCSprite* m_BackgroundSprite;
    GJGroundLayer* m_GJGroundLayer;
    CCMenu* m_shitcodingmenu;
    CCPoint m_shitcodingmenuPoint;
    CCMenu* m_shitdotingmenu;
    CCMenuItem* m_rightTouchZone;
    CCMenuItem* m_leftTouchZone;
    int m_page;
    bool m_waitForTouchFromLeft;
    bool m_waitForTouchFromRight;
    void updSchedule(float) {
        //swipe
        if (this->getChildByIDRecursive("scroll")) {
            auto scroll = typeinfo_cast<ScrollLayer*>(this->getChildByIDRecursive("scroll"));
            auto asd = scroll->m_contentLayer;
            //>>>>>> = <[]
            if (asd->getPositionX() > 70.f) {
                scroll->setTouchEnabled(0);
                asd->stopAllActions();
                asd->setPositionX(0.f);
                onPrev(asd);
            }
            //<<<<<< =  []>
            else if (asd->getPositionX() < -70.f) {
                scroll->setTouchEnabled(0);
                asd->stopAllActions();
                asd->setPositionX(0.f);
                onNext(asd);
            }
            scroll->setTouchEnabled(1);
            scroll->setTouchPriority(-1);
        }
    }
    void theSchedule(float) {
        //animate
        if (this->m_fields->m_shitcodingmenu->getPosition() != this->m_fields->m_shitcodingmenuPoint) {
            if (!this->m_fields->m_shitcodingmenu->getActionByTag(m_fields->m_page)) {
                auto moveTo = CCEaseElasticOut::create(CCMoveTo::create(0.8f, this->m_fields->m_shitcodingmenuPoint), 0.6f);
                moveTo->setTag(m_fields->m_page);
                this->m_fields->m_shitcodingmenu->stopAllActions();
                this->m_fields->m_shitcodingmenu->runAction(moveTo);
            }
        }
        //dots colorize
        if (this->m_fields->m_shitdotingmenu) {
            for (int i = 0; i < this->m_fields->m_shitdotingmenu->getChildrenCount(); i++) {
                CCNode* thisdot = this->m_fields->m_shitdotingmenu->getChildByTag(i);
                if (thisdot->getTag() == this->m_fields->m_page) thisdot->runAction(CCTintTo::create(0.1f, 100, 100, 100));
                else thisdot->runAction(CCTintTo::create(0.1f, 255, 255, 255));
            }
        };
        //wah update page on real scroll
        {
            m_fields->m_LevelsScrollLayer->instantMoveToPage(m_fields->m_page);
        }
    }
    void controlAndAddStuff() {
        //hide away this unexplored thing
        this->m_fields->m_LevelsScrollLayer->setVisible(0);
        this->m_fields->m_LevelsScrollLayer->setTouchEnabled(0);
        //add updates
        this->schedule(schedule_selector(LevelSelectLayerExt::updSchedule));
        this->schedule(schedule_selector(LevelSelectLayerExt::theSchedule), 0.1f);
        //m_shitcodingmenu
        {
            this->m_fields->m_shitcodingmenu = CCMenu::create();
            this->m_fields->m_shitcodingmenu->setPosition(CCPointZero);
            this->m_fields->m_shitcodingmenu->setAnchorPoint(CCPointZero);
            this->addChild(this->m_fields->m_shitcodingmenu);
            //add levels
            int i = start_from;
            for (i; i <= cmp_amount; i++) {
                auto lvl = GameLevelManager::sharedState()->getMainLevel(i, 0);
                auto page = LevelPage::create(lvl);
                page->updateDynamicPage(lvl);
                this->m_fields->m_shitcodingmenu->addChild(page, i, i);
            }
            auto lvl = GJGameLevel::create();
            //thetowerpage
            if (not Mod::get()->getSettingValue<bool>("NO_THETOWER")) {
                lvl->m_levelID = -2;
                auto thetowerpage = LevelPage::create(lvl);
                thetowerpage->updateDynamicPage(lvl);
                this->m_fields->m_shitcodingmenu->addChild(thetowerpage, i, i);
                i++;
            };
            //cumingsoon
            if (not Mod::get()->getSettingValue<bool>("NO_COMINGSOON")) {
                lvl->m_levelID = -1;
                auto cumingsoon = LevelPage::create(lvl);
                cumingsoon->updateDynamicPage(lvl);
                auto COMINGSOON_TEXT = (Mod::get()->getSettingValue<std::string>("COMINGSOON_TEXT"));
                CCObject* obj = nullptr;
                CCARRAY_FOREACH(cumingsoon->getChildren(), obj)
                {
                    auto node = dynamic_cast<CCNode*>(obj);
                    if (node) {
                        //pBackgroundSprite
                        auto label = typeinfo_cast<CCLabelBMFont*>(obj);
                        if (label) {
                            label->setString(COMINGSOON_TEXT.data());
                        }
                    }
                }
                this->m_fields->m_shitcodingmenu->addChild(cumingsoon, i, i);
            };
            //Layout
            this->m_fields->m_shitcodingmenu->setLayout(
                ColumnLayout::create()
                ->setAxisAlignment(AxisAlignment::Start)
                ->setCrossAxisAlignment(AxisAlignment::Start)
                ->setCrossAxisLineAlignment(AxisAlignment::Start)
                ->setGrowCrossAxis(true)
                ->setCrossAxisReverse(true)
                ->setGap(0.f)
            );
            this->m_fields->m_shitcodingmenu->updateLayout();
            //scroll to initpage
            instantMoveToCurrentPage();
        };
        //nav dots
        {
            auto shitdotingmenu = CCMenu::create();
            this->m_fields->m_shitdotingmenu = shitdotingmenu;
            shitdotingmenu->setPosition(CCPointZero);
            this->addChild(shitdotingmenu);
            //add pages
            for (int i = 0; i < this->m_fields->m_shitcodingmenu->getChildrenCount(); i++) {
                auto dot = CCSprite::createWithSpriteFrameName("uiDot_001.png");
                dot->setScale(1.15f);
                dot->setBlendFunc({ GL_SRC_ALPHA, GL_ONE });
                CCMenuItemSpriteExtra* CCMenuItemSpriteExtra_ = CCMenuItemSpriteExtra::create(
                    dot, this, menu_selector(LevelSelectLayerExt::onDot)
                );
                CCMenuItemSpriteExtra_->setSizeMult(1.5f);
                shitdotingmenu->addChild(CCMenuItemSpriteExtra_, 0, i);
            }
            shitdotingmenu->alignItemsHorizontallyWithPadding(6.0f);
            shitdotingmenu->setPosition(
                CCPoint(CCDirector::get()->getWinSize().width * 0.5f, CCDirector::get()->getScreenBottom() + 16.000f)
            );
        };
        //scrollin shit
        {
            auto asd = geode::ScrollLayer::create(CCDirector::get()->getWinSize(), 1, 0);
            asd->setID("scroll");
            this->addChild(asd);
        }
    }
    void mainSetup(int p0) {
        m_fields->m_page = p0;
        //getNodes
        CCObject* obj;
        CCARRAY_FOREACH(this->getChildren(), obj)
        {
            auto node = dynamic_cast<CCNode*>(obj);
            if (node) {
                auto list = typeinfo_cast<BoomScrollLayer*>(obj);
                if (list) this->m_fields->m_LevelsScrollLayer = list;
                auto pGJGroundLayer = typeinfo_cast<GJGroundLayer*>(obj);
                if (pGJGroundLayer) this->m_fields->m_GJGroundLayer = pGJGroundLayer;
                auto sprite = typeinfo_cast<CCSprite*>(obj);
                if (sprite) {
                    if(framePath(sprite).find("GJ_gradientBG") != std::string::npos) this->m_fields->m_BackgroundSprite = sprite;
                }
            }
        }
        controlAndAddStuff();
    }
    void instantMoveToCurrentPage() {
        float newPointX = 0.f;
        for (int i = 0; i < m_fields->m_page; i++) {
            newPointX = newPointX - CCDirector::get()->getScreenRight();
        };
        m_fields->m_shitcodingmenuPoint.x = newPointX;
        m_fields->m_shitcodingmenu->setPositionX(newPointX);
    }
    void onDot(cocos2d::CCObject* sender) {
        bool isPrev = m_fields->m_page < sender->getTag();
        m_fields->m_page = sender->getTag();
        instantMoveToCurrentPage();
    }
    void onNext(cocos2d::CCObject* sender) {
        if (Mod::get()->getSettingValue<bool>("USE_PATCHES")) return LevelSelectLayer::onNext(sender);
        ++m_fields->m_page;
        if (m_fields->m_page >= m_fields->m_shitcodingmenu->getChildrenCount()) {
            m_fields->m_page = 0;
            m_fields->m_shitcodingmenuPoint.x = 0;
            m_fields->m_shitcodingmenu->setPositionX(0 + CCDirector::get()->getScreenRight());
            return;
        };
        auto newPointX = this->m_fields->m_shitcodingmenuPoint.x - CCDirector::get()->getScreenRight();
        if (sender) {
            m_fields->m_shitcodingmenuPoint.x = (newPointX);
        }
        else {
            m_fields->m_shitcodingmenu->setPositionX(newPointX);
            m_fields->m_shitcodingmenuPoint.x = (newPointX);
        }
    };
    void onPrev(cocos2d::CCObject* sender) {
        if (Mod::get()->getSettingValue<bool>("USE_PATCHES")) return LevelSelectLayer::onPrev(sender);
        --m_fields->m_page;
        if (m_fields->m_page < 0) {
            m_fields->m_page = m_fields->m_shitcodingmenu->getChildrenCount() - 1;
            //get page point
            float newPointX = 0.f;
            for (int i = 0; i < m_fields->m_page; i++) {
                newPointX = newPointX - CCDirector::get()->getScreenRight();
            };
            m_fields->m_shitcodingmenuPoint.x = newPointX;
            m_fields->m_shitcodingmenu->setPositionX(newPointX - CCDirector::get()->getScreenRight());
            return;
        };
        auto newPointX = this->m_fields->m_shitcodingmenuPoint.x + CCDirector::get()->getScreenRight();
        if (sender) {
            m_fields->m_shitcodingmenuPoint.x = (newPointX);
        }
        else {
            m_fields->m_shitcodingmenu->setPositionX(newPointX);
            m_fields->m_shitcodingmenuPoint.x = (newPointX);
        }
    };
    void onInfo(cocos2d::CCObject* sender) {
        if (Mod::get()->getSettingValue<bool>("USE_PATCHES")) return LevelSelectLayer::onInfo(sender);
        auto currentPage = dynamic_cast<LevelPage*>(m_fields->m_shitcodingmenu->getChildByTag(m_fields->m_page + 1));
        if (currentPage) currentPage->onInfo(currentPage);
    }
    static LevelSelectLayer* create(int p0) {
        auto rtn = LevelSelectLayer::create(p0);
        if (Mod::get()->getSettingValue<bool>("USE_PATCHES")) return rtn;

        ((LevelSelectLayerExt*)rtn)->mainSetup(p0);

        return rtn;
    }
};