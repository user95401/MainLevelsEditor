#include <_main.hpp>

#include <Geode/modify/LevelPage.hpp>
class $modify(LevelPageExt, LevelPage) {
    void editLevel(CCObject*) {
        auto ae = MLE_UI::LevelConfigPopup::create(this->m_level);
        ae->m_onSave = [ae]() {
            CCDirector::get()->pushScene(CCTransitionCrossFade::create(
                0.15f, LevelSelectLayer::scene(ae->m_level->m_levelID.value() - 1)
            )); 
        };
        ae->show();
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
            //remove meta
            std::remove(levelMetaFile.string().data());
            //remove data
            std::remove(levelFile.string().data());
            //update settings here
            Mod::get()->setSettingValue("max_id", SETTING(int64_t, "max_id") - 1);
            //change ids of all next levels
            {
                //level meta
                for (int levelID = 0; levelID < 500; levelID++) {
                    if (levelID > currentID and levelID < 500) {
                        auto rn1 = levels_meta_path / fmt::format("{}.json", levelID);
                        auto rn2 = levels_meta_path / fmt::format("{}.json", levelID - 1);
                        if (my_fs::exists(rn1)) my_fs::rename(rn1, rn2);
                    };
                }
                //level data
                for (int levelID = 0; levelID < 500; levelID++) {
                    if (levelID > currentID) {
                        auto rn1 = levels_path / fmt::format("{}.txt", levelID);
                        auto rn2 = levels_path / fmt::format("{}.txt", levelID - 1);
                        if (my_fs::exists(rn1)) my_fs::rename(rn1, rn2);
                    }
                }
            }
            //reset layer
            CCDirector::get()->pushScene(CCTransitionCrossFade::create(
                0.15f, LevelSelectLayer::scene(currentID - 1)
            ));
        });
    }
    $override bool init(GJGameLevel* p0) {
        auto rtn = LevelPage::init(p0);
        if (SETTING(bool, "ui")) {
            CCMenuItemSpriteExtra* deleteLevel; {
                deleteLevel = CCMenuItemSpriteExtra::create(MLE_UI::DeleteButtonSprite(), this, menu_selector(LevelPageExt::deleteLevel));
                deleteLevel->setID("deleteLevel"_spr);
                deleteLevel->setPosition(CCPoint(190, 30));
                deleteLevel->m_baseScale = 0.8f;
                deleteLevel->setScale(deleteLevel->m_baseScale);
            };
            CCMenuItemSpriteExtra* editLevel; {
                editLevel = CCMenuItemSpriteExtra::create(MLE_UI::SettingsButtonSprite(), this, menu_selector(LevelPageExt::editLevel));
                editLevel->setID("editLevel"_spr);
                editLevel->setPosition(CCPoint(190, 0));
                editLevel->m_baseScale = 0.8f;
                editLevel->setScale(editLevel->m_baseScale);
            };
            if (auto levelMenu = getChildByIDRecursive("level-menu")) {
                levelMenu->addChild(deleteLevel);
                levelMenu->addChild(editLevel);
            };
        }
        return rtn;
    };
    $override void updateDynamicPage(GJGameLevel* p0) {
        LevelPage::updateDynamicPage(p0);
        //my menu_for_level_page
        if (auto pCCMenu = this->getChildByIDRecursive("menu_for_level_page"_spr)) {
            pCCMenu->setVisible(p0->m_levelID >= 1);
            pCCMenu->setZOrder(p0->m_levelID);
        }
    }
};