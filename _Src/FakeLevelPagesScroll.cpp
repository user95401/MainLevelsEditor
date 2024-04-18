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
    void updSchedule(float) {
        //ground color
        /*this->m_fields->m_GJGroundLayer->updateGround01Color(ccColor3B(
            this->m_fields->m_BackgroundSprite->getColor().r,
            this->m_fields->m_BackgroundSprite->getColor().g,
            this->m_fields->m_BackgroundSprite->getColor().b
        ));*/
    }
    void theSchedule(float) {
        //color
        /*auto pageColor = this->colorForPage(m_fields->m_page);
        this->m_fields->m_BackgroundSprite->runAction(CCTintTo::create(0.050f, pageColor.r, pageColor.g, pageColor.b));
        *///animate
        if (this->m_fields->m_shitcodingmenu->getPosition() != this->m_fields->m_shitcodingmenuPoint) {
            if (!this->m_fields->m_shitcodingmenu->getActionByTag(m_fields->m_page)) {
                auto moveTo = CCEaseElasticOut::create(CCMoveTo::create(0.5f, this->m_fields->m_shitcodingmenuPoint), 0.6f);
                moveTo->setTag(m_fields->m_page);
                this->m_fields->m_shitcodingmenu->stopAllActions();
                this->m_fields->m_shitcodingmenu->runAction(moveTo);
            }
        }
        //dots colorize
        if (this->m_fields->m_shitdotingmenu) {
            for (int i = 0; i < this->m_fields->m_shitdotingmenu->getChildrenCount(); i++) {
                CCNode* thisdot = this->m_fields->m_shitdotingmenu->getChildByTag(i);
                if (thisdot->getTag() == this->m_fields->m_page) thisdot->runAction(CCTintTo::create(0.1f, 160, 160, 160));
                else thisdot->runAction(CCTintTo::create(0.1f, 255, 255, 255));
            }
        };
        //wah
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
        //a
        //levels
        this->m_fields->m_shitcodingmenu = CCMenu::create();
        this->m_fields->m_shitcodingmenu->setPosition(CCPointZero);
        this->m_fields->m_shitcodingmenu->setAnchorPoint(CCPointZero);
        this->addChild(this->m_fields->m_shitcodingmenu);
        //add levels
        for (int i = start_from; i <= cmp_amount; i++) {
            auto lvl = GameLevelManager::sharedState()->getMainLevel(i, 0);
            auto page = LevelPage::create(lvl);
            page->updateDynamicPage(lvl);
            this->m_fields->m_shitcodingmenu->addChild(page, i, i);
        }
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
        //nav dots
        auto shitdotingmenu = CCMenu::create();
        this->m_fields->m_shitdotingmenu = shitdotingmenu;
        shitdotingmenu->setPosition(CCPointZero);
        this->addChild(shitdotingmenu);
        //add pages
        for (int i = 0; i < this->m_fields->m_shitcodingmenu->getChildrenCount(); i++) {
            auto dot = CCSprite::createWithSpriteFrameName("uiDot_001.png");
            dot->setScale(1.15f);
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
        ++m_fields->m_page;
        if (m_fields->m_page >= m_fields->m_shitcodingmenu->getChildrenCount()) {
            m_fields->m_page = 0;
            m_fields->m_shitcodingmenuPoint.x = 0;
            m_fields->m_shitcodingmenu->setPositionX(0 - CCDirector::get()->getScreenRight());
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
        --m_fields->m_page;
        if (m_fields->m_page < 0) {
            m_fields->m_page = m_fields->m_shitcodingmenu->getChildrenCount() - 1;
            //get page point
            float newPointX = 0.f;
            for (int i = 0; i < m_fields->m_page; i++) {
                newPointX = newPointX - CCDirector::get()->getScreenRight();
            };
            m_fields->m_shitcodingmenuPoint.x = newPointX;
            m_fields->m_shitcodingmenu->setPositionX(newPointX + CCDirector::get()->getScreenRight());
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
    virtual void keyDown(cocos2d::enumKeyCodes p0) {
        if (p0 == KEY_Left and m_fields->m_shitcodingmenu->numberOfRunningActions() == 0) onPrev(this);
        if (p0 == KEY_Right and m_fields->m_shitcodingmenu->numberOfRunningActions() == 0) onNext(this);
        return LevelSelectLayer::keyDown(p0);
    }
    static LevelSelectLayer* create(int p0) {
        auto rtn = LevelSelectLayer::create(p0);
        ((LevelSelectLayerExt*)rtn)->mainSetup(p0);
        return rtn;
    }
};