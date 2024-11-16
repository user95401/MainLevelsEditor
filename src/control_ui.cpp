#include <_main.hpp>

#include <Geode/modify/LevelPage.hpp>
class $modify(LevelPageExt, LevelPage) {
    void editLevel(CCObject*) {
        auto ae = mle_ui::LevelConfigPopup::create(this->m_level);
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
                        if (fs::exists(rn1)) fs::rename(rn1, rn2);
                    };
                }
                //level data
                for (int levelID = 0; levelID < 500; levelID++) {
                    if (levelID > currentID) {
                        auto rn1 = levels_path / fmt::format("{}.txt", levelID);
                        auto rn2 = levels_path / fmt::format("{}.txt", levelID - 1);
                        if (fs::exists(rn1)) fs::rename(rn1, rn2);
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
                deleteLevel = CCMenuItemSpriteExtra::create(mle_ui::deleteButtonSprite(), this, menu_selector(LevelPageExt::deleteLevel));
                deleteLevel->setID("deleteLevel"_spr);
                deleteLevel->m_baseScale = 0.8f;
                deleteLevel->setScale(deleteLevel->m_baseScale);
            };
            CCMenuItemSpriteExtra* editLevel; {
                editLevel = CCMenuItemSpriteExtra::create(mle_ui::settingsButtonSprite(), this, menu_selector(LevelPageExt::editLevel));
                editLevel->setID("editLevel"_spr);
                editLevel->m_baseScale = 0.8f;
                editLevel->setScale(editLevel->m_baseScale);
            };
            if (auto levelMenu = getChildByIDRecursive("level-menu")) {
                //levelMenu->addChild(deleteLevel);
                //levelMenu->addChild(editLevel);
                {
                    levelMenu->addChild(editLevel);

                    auto view = CCDirector::get()->getVisibleSize();
                    auto viewCenter = view / 2;
                    auto worldPos = viewCenter + CCPointMake((view.width / 2) - 68, 90);
                    editLevel->setPosition(levelMenu->convertToNodeSpace(worldPos));
                }
                {
                    levelMenu->addChild(deleteLevel);

                    auto view = CCDirector::get()->getVisibleSize();
                    auto viewCenter = view / 2;
                    auto worldPos = viewCenter + CCPointMake((view.width / 2) - 68, 60);
                    deleteLevel->setPosition(levelMenu->convertToNodeSpace(worldPos));
                }
            };
        }
        return rtn;
    };
    $override void updateDynamicPage(GJGameLevel* p0) {
        LevelPage::updateDynamicPage(p0);
    }
};

#include <Geode/modify/FLAlertLayer.hpp>
class $modify(SongInfoLayerExt, FLAlertLayer) {
    auto onConfigureSong(CCObject*) {
        auto ae = mle_ui::AudioConfigPopup::create(this->getTag());
        ae->m_onSave = [this, ae]() {
            this->keyBackClicked();
            ae->m_audio.openInfoLayer();
            if (auto openedSongsLayer = cocos::findFirstChildRecursive<SongsLayer>(CCDirector::get()->m_pRunningScene, [](auto) {return true; })) {
                auto scrollLayer = (ScrollLayer*)openedSongsLayer->m_listLayer->getChildByID("scrollLayer"_spr);
                auto customListView = cocos::findFirstChildRecursive<CustomListView>(openedSongsLayer->m_listLayer, [](auto e) { return true; });
                if (scrollLayer) scrollLayer->removeFromParent();
                if (customListView) customListView->removeFromParent();
                openedSongsLayer->customSetup(); 
                auto newScrollLayer = (ScrollLayer*)openedSongsLayer->m_listLayer->getChildByID("scrollLayer"_spr);
                newScrollLayer->m_contentLayer->setPosition(scrollLayer->m_contentLayer->getPosition());
            }
            };
        ae->show();
    }
    auto onCopySong(CCObject*) {
        auto pCopyAudioPopup = mle_ui::CopyAudioPopup::create(this);
        pCopyAudioPopup->show();
    }
    auto songInfoLayerSetupSch(float) {
        //add the button ya?
        if (SETTING(bool, "ui")) {
            if (findDataNode(this, "m_isRobtopSong")) {
                CCMenuItemSpriteExtra* settings;
                settings = CCMenuItemSpriteExtra::create(mle_ui::settingsButtonSprite(), this, menu_selector(SongInfoLayerExt::onConfigureSong));
                settings->setID("settings"_spr);
                settings->setPosition(CCPoint(382.000f, -50.f));
                settings->m_baseScale = 0.8f;
                settings->setScale(settings->m_baseScale); 
                this->m_buttonMenu->addChild(settings);
            }
            else {
                CCMenuItemSpriteExtra* copy;
                copy = CCMenuItemSpriteExtra::create(
                    CCSprite::createWithSpriteFrameName("GJ_copyBtn2_001.png"), 
                    this, menu_selector(SongInfoLayerExt::onCopySong)
                );
                copy->setID("copy"_spr);
                copy->setPosition(CCPoint(382.000f, -50.f));
                copy->m_baseScale = 0.8f;
                copy->setScale(copy->m_baseScale);
                this->m_buttonMenu->addChild(copy);
            }
        }
    }
    $override void show() {
        FLAlertLayer::show();
        if (typeinfo_cast<SongInfoLayer*>(this)) //schedule is for data nodes setup!!!
            this->scheduleOnce(schedule_selector(SongInfoLayerExt::songInfoLayerSetupSch), 0.f);
    }
};

#include <Geode/modify/PauseLayer.hpp>
class $modify(PauseLayerExt, PauseLayer) {
    void copyLevel(CCObject*) {
        auto pCopyLevelPopup = mle_ui::CopyLevelPopup::create(
            GameManager::get()->getPlayLayer()->m_level
        );
        pCopyLevelPopup->show();
    }
    $override void customSetup() {
        PauseLayer::customSetup();
        if (not SETTING(bool, "ui")) return;
        if (auto menu = typeinfo_cast<CCMenu*>(this->getChildByIDRecursive("right-button-menu"))) {
            CCMenuItemSpriteExtra* copyLevel; {
                copyLevel = CCMenuItemSpriteExtra::create(
                    CCSprite::createWithSpriteFrameName("GJ_copyBtn_001.png"),
                    this, menu_selector(PauseLayerExt::copyLevel)
                );
                copyLevel->setID("copyLevel"_spr);
                copyLevel->m_baseScale = 0.575f;
                copyLevel->setScale(copyLevel->m_baseScale);
                copyLevel->setLayoutOptions(AxisLayoutOptions::create()->setAutoScale(false));
                menu->addChild(copyLevel);
                menu->updateLayout();
            };
        }
    }
};