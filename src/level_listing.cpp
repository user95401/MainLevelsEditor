#include <_main.hpp>

#include <Geode/modify/BoomScrollLayer.hpp>
class $modify(BoomScrollLayerExt, BoomScrollLayer) {
    static BoomScrollLayer* create(cocos2d::CCArray* pages, int unk1, bool unk2, cocos2d::CCArray* unk3, DynamicScrollDelegate* delegate) {
        /*log::debug(
            "\n{}(\ncocos2d::CCArray* {},\n int {}, bool {},\n cocos2d::CCArray* {},\n DynamicScrollDelegate* {}\n)\n",
            __FUNCTION__, pages, unk1, unk2, unk3, (CCObject*)delegate
        );*/
        if (auto mainLevelsLayer = typeinfo_cast<LevelSelectLayer*>(delegate)) {
            //mainLevelsLayer->setRotation(12.f);
            //yeaa pages is standing for 3 level pages (dont touch them :D)
            //unk3 contains level nodes.

            auto myLevelsArray = CCArray::create();

            //add levels range
            int id = SETTING(int64_t, "start_id");
            int max_id = SETTING(int64_t, "max_id");
            for (id; id <= max_id; id++)
                myLevelsArray->addObject(LevelTools::getLevel(id, 0));

            //special_ids
            for (auto strID : string::explode(",", SETTING(std::string, "special_ids"))) {
                auto id_num = utils::numFromString<int>(strID);
                if (id_num.isOk()) 
                    myLevelsArray->addObject(LevelTools::getLevel(id_num.unwrapOrDefault(), 0));
            }

            //the_tower
            if (SETTING(bool, "the_tower")) myLevelsArray->addObject(unk3->objectAtIndex(unk3->count() - 2));

            //cumingson
            if (SETTING(bool, "coming_soon")) myLevelsArray->addObject(unk3->objectAtIndex(unk3->count() - 1));

            return BoomScrollLayer::create(pages, unk1, unk2, myLevelsArray, mainLevelsLayer);
        }
        return BoomScrollLayer::create(pages, unk1, unk2, unk3, delegate);
    }
};

#include <Geode/modify/LevelPage.hpp>
class $modify(LevelPage) {
    $override void updateDynamicPage(GJGameLevel* p0) {
        LevelPage::updateDynamicPage(p0);
        //difficultySprite
        if (auto difficultySprite = typeinfo_cast<CCSprite*>(this->getChildByIDRecursive("difficulty-sprite"))) {
            std::string frameName = GJDifficultySprite::getDifficultyFrame((int)p0->m_difficulty, GJDifficultyName::Short).data();
            frameName = std::regex_replace(frameName, std::regex("difficulty"), "diffIcon");
            if (auto frame = CCSpriteFrameCache::get()->spriteFrameByName(frameName.data()))
                difficultySprite->setDisplayFrame(frame);
        }
    }
};

#include <Geode/modify/CCLayer.hpp>
class $modify(LevelAreaLayerExt, CCLayer) {
    void onBack(CCObject*) {
        GameManager::get()->playMenuMusic();
        auto levelsCount = SETTING(int64_t, "max_id");
        auto specialLevels = string::explode(",", SETTING(std::string, "special_ids"));
        auto specialLevelsCount = specialLevels[0].empty() ? 0 : specialLevels.size();
        switchToScene(LevelSelectLayer::create(
            levelsCount + specialLevelsCount
        ));
    }
    auto customSetupSch(float) {
        cocos::findFirstChildRecursive<CCMenuItemSpriteExtra>(
            this, [](auto node) {
                if (cocos::getChildBySpriteFrameName(node, "GJ_arrow_03_001.png") == nullptr) return false;
                
                node->m_pfnSelector = menu_selector(LevelAreaLayerExt::onBack);
                if (node->getID() == "") node->setID("onBackBtnPatch"_spr);
                
                return true;
            }
        );
    }
    $override bool init() {
        if (typeinfo_cast<LevelAreaLayer*>(this)) 
            this->scheduleOnce(schedule_selector(LevelAreaLayerExt::customSetupSch), 0.f);
        return CCLayer::init();
    }
};
#include <Geode/modify/LevelAreaLayer.hpp>
class $modify(LevelAreaLayer) {
    $override void keyBackClicked() {
        ((LevelAreaLayerExt*)LevelAreaLayerExt::create())->onBack(this);
    }
};