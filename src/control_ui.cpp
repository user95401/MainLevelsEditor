#include <_main.hpp>

#include <Geode/modify/LevelPage.hpp>
class $modify(LevelPageExt, LevelPage) {
    void updateUI(float) {
        if (auto pCCMenu = this->getChildByIDRecursive("menu_for_level_page"_spr)) {
            pCCMenu->setVisible(this->m_level->m_levelID >= 1);
            pCCMenu->setZOrder(this->m_level->m_levelID);
        }
    }
    void editLevel(CCObject*) {
    }
    void deleteLevel(CCObject*) {
        auto currentLevel = this->m_level;
        auto currentID = this->m_level->m_levelID.value();
        auto levelFile = levels_path / fmt::format("{}.txt", currentID);
        auto levelMetaFile = levels_meta_path / fmt::format("{}.json", currentID);
        auto content = fmt::format(
            "<cj>ID:</c> {}" "\n"
            "<cg>Name:</c> {}" "\n"
            "<cr>Delete</c> this level from <co>mod config</c>?",
            currentID,
            currentLevel->m_levelName.c_str(),
            (int)currentLevel->m_difficulty
        );
        geode::createQuickPopup("Delete Level", content, "No", "Yes", [this, levelFile, levelMetaFile, currentID, currentLevel](void*, bool asd) {
            if (!asd) return;
            //level setup
            std::remove(levelMetaFile.string().data());
            //data store
            std::remove(levelFile.string().data());
            //_PagesSetupPatch
            {
                //update settings here
                Mod::get()->setSettingValue("max_id", SETTING(int64_t, "max_id") - 1);
            };
            //_AudioTracks
            {
                //idk lol
            };
            //asshell
            {
                //level setup
                for (int levelID = 0; levelID < 500; levelID++) {
                    if (levelID > currentID and levelID < 500) {
                        auto rn1 = levels_meta_path / fmt::format("{}.json", levelID);
                        auto rn2 = levels_meta_path / fmt::format("{}.json", levelID - 1);
                        my_fs::rename(rn1, rn2);
                    };
                }
                //level data
                for (int levelID = 0; levelID < 500; levelID++) {
                    if (levelID > currentID) {
                        auto rn1 = levels_path / fmt::format("{}.txt", levelID);
                        auto rn2 = levels_path / fmt::format("{}.txt", levelID - 1);
                        my_fs::rename(rn1, rn2);
                    }
                }
            }
            auto newLevelSelectLayerScene = LevelSelectLayer::scene(currentID - 1);
            CCDirector::get()->pushScene(CCTransitionCrossFade::create(0.1f, newLevelSelectLayerScene));
            });
    }
    bool init(GJGameLevel* p0) {
        auto rtn = LevelPage::init(p0);
        if (SETTING(bool, "ui")) {
            this->schedule(schedule_selector(LevelPageExt::updateUI));
            CCMenuItemSpriteExtra* deleteLevel;
            {
                auto hi = geode::AccountButtonSprite::create(
                    CCSprite::createWithSpriteFrameName("edit_delBtn_001.png"),
                    AccountBaseColor::Gray
                );
                hi->setScale(0.8f);
                typeinfo_cast<CCSprite*>(hi->getChildren()->objectAtIndex(0))->setScale(1.1f);
                deleteLevel = CCMenuItemSpriteExtra::create(hi, this, menu_selector(LevelPageExt::deleteLevel));
                deleteLevel->setID("deleteLevel"_spr);
                deleteLevel->setPosition(CCPoint(190, 30));
                deleteLevel->m_baseScale = 0.8f;
                deleteLevel->setScale(deleteLevel->m_baseScale);
            };
            CCMenuItemSpriteExtra* editLevel;
            {
                auto hi = geode::AccountButtonSprite::create(
                    CCSprite::createWithSpriteFrameName("d_cogwheel_04_001.png"),
                    AccountBaseColor::Gray
                );
                hi->setScale(0.8f);
                typeinfo_cast<CCSprite*>(hi->getChildren()->objectAtIndex(0))->setScale(1.1f);
                typeinfo_cast<CCSprite*>(hi->getChildren()->objectAtIndex(0))->setColor({ 20, 20, 20 });
                hi->addChild(typeinfo_cast<CCSprite*>(hi->getChildren()->objectAtIndex(0)));//make it darker
                editLevel = CCMenuItemSpriteExtra::create(hi, this, menu_selector(LevelPageExt::editLevel));
                editLevel->setID("editLevel"_spr);
                editLevel->setPosition(CCPoint(190, 0));
                editLevel->m_baseScale = 0.8f;
                editLevel->setScale(editLevel->m_baseScale);
            };
            auto levelMenu = getChildByIDRecursive("level-menu");
            {
                levelMenu->addChild(deleteLevel);
                levelMenu->addChild(editLevel);
            };
        }
        return rtn;
    };
};