#include "_main.hpp"

#include <Geode/modify/EditorPauseLayer.hpp>
class $modify(EditorPauseLayerExt, EditorPauseLayer) {
    $override void saveLevel() {
        EditorPauseLayer::saveLevel();
        if (not this->m_editorLayer->getChildByIDRecursive("IsMainLevelEditor")) return;
        this->m_editorLayer->m_level->m_levelType = GJLevelType::Local;
        mle_leveltools::updateLevelDataAndMetaFiles(
            this->m_editorLayer->getLevelString(),
            this->m_editorLayer->m_level
        );
    }
    $override bool init(LevelEditorLayer * p0) {
        auto rtn = EditorPauseLayer::init(p0);
        if (not p0->getChildByIDRecursive("IsMainLevelEditor")) return rtn;
        if (auto node = this->getChildByIDRecursive("save-and-play-button")) 
            node->removeFromParent();
        if (auto node = (CCMenu*)this->getChildByIDRecursive("resume-menu")) {
            node->addChild(TextArea::create(
                "<co>Main Level Editor</c>\n"
                "Actually <cg>saves level data</c> to file\n"
                "<cy>in user95401.mainlevelseditor config</c>.\n"
                "<co>User coins</c> will be <cg>saved as</c> <cy>Secret Coins</c>!\n"
                "<cr>Btw don't forget set Unique ID (in \"Edit Special\")</c>\n"
                " "
                , "chatFont.fnt", 1.f, 600.f, { 0.5f, 0.5f }, 14.f, 0
            ), -1, 6302);
            if (auto firstLine = cocos::findFirstChildRecursive<CCLabelBMFont>(node->getChildByTag(6302), [](auto) { return true; })) {
                firstLine->setScale(1.5f);
                firstLine->setAnchorPoint({
                    firstLine->getAnchorPoint().x + (firstLine->getScale() / 10),
                    firstLine->getAnchorPoint().y
                    });
            }
            node->getChildByTag(6302)->setScale(0.8f);
            node->getChildByTag(6302)->setLayoutOptions(AxisLayoutOptions::create()->setAutoScale(0));
            node->updateLayout();
        }
        return rtn;
    }
};

#include <Geode/modify/EditorUI.hpp>
class $modify(EditorUIExt, EditorUI) {
    bool init(LevelEditorLayer* editorLayer) {
        auto rtn = EditorUI::init(editorLayer);

        //wasnt bindings for that yet
        if (auto editSpecialButton = typeinfo_cast<CCMenuItemSpriteExtra*>(this->getChildByIDRecursive("edit-special-button"))) {
            auto& org_pfnSelector = editSpecialButton->m_pfnSelector;
            auto& org_pListener = editSpecialButton->m_pListener;
            CCMenuItemExt::assignCallback<CCMenuItemSpriteExtra>(
                editSpecialButton, [this, editSpecialButton, org_pfnSelector, org_pListener](
                    CCMenuItemSpriteExtra* item) {
                        if (not this->m_selectedObject) return (org_pListener->*org_pfnSelector)(editSpecialButton);
                        if (not this->m_editorLayer->getChildByIDRecursive("IsMainLevelEditor")) {
                            return (org_pListener->*org_pfnSelector)(editSpecialButton);
                        }

                        //user coin id 1329, 142 secret coin
                        if (this->m_selectedObject->m_objectID == 1329) {
                            this->m_selectedObject->m_objectID = 142;
                            this->editObject(item);
                            this->m_selectedObject->m_objectID = 1329;
                            return;
                        }

                        (org_pListener->*org_pfnSelector)(editSpecialButton);
                }
            );

        }

        return rtn;
    }
};

#include <Geode/modify/FLAlertLayer.hpp>
class $modify(LevelSettingsLayerExt, FLAlertLayer) {
    auto audioIdFromLabel(CCLabelBMFont * songLabel) {
        auto str_expl = string::explode(":", songLabel->getString());
        auto audioID = utils::numFromString<int>(str_expl[0]).unwrapOr(0);
        return audioID;
    }
    auto getCurrentAudioNode() {
        auto audioList = this->getChildByID("audioList");
        return cocos::findFirstChildRecursive<CCLabelBMFont>(
            audioList,
            [](CCLabelBMFont* node) { return node->isVisible(); }
            );
    }
    void updateLabel(CCLabelBMFont* pSongLabel = nullptr, bool touchStr = 1) {
        auto songLabel = pSongLabel ? pSongLabel : typeinfo_cast<CCLabelBMFont*>(this->getChildByIDRecursive("songLabel"));
        //str
        if (touchStr) songLabel->setString(getCurrentAudioNode()->getString());
        //scale
        songLabel->setScale(198.0f / songLabel->getContentWidth());
        if (songLabel->getScale() > 0.8f) songLabel->setScale(0.8f);
    }
    void onPrevAudio(CCObject*) {
        if (this == nullptr) return;
        auto audioList = this->getChildByID("audioList")->getChildren();
        //get near node index
        auto currentIndex = audioList->indexOfObject(getCurrentAudioNode());
        auto newIndex = currentIndex <= 0 ? 0 : currentIndex - 1;//no negative index for ccarray!!!
        //get node
        auto newCurrentAudioNode = typeinfo_cast<CCLabelBMFont*>(audioList->objectAtIndex(newIndex));
        if (not newCurrentAudioNode) return log::warn("newCurrentAudioNode={}", newCurrentAudioNode);
        //select other node
        getCurrentAudioNode()->setVisible(0);
        newCurrentAudioNode->setVisible(1);
        //set audio
        auto editorLayer = GameManager::get()->getEditorLayer();
        editorLayer->m_level->m_audioTrack = newCurrentAudioNode->getTag();
        editorLayer->m_level->m_audioTrack = newCurrentAudioNode->getTag();
        updateLabel();
    }
    void onNextAudio(CCObject*) {
        if (this == nullptr) return;
        auto audioList = this->getChildByID("audioList")->getChildren();
        auto songLabel = (CCLabelBMFont*)this->getChildByIDRecursive("songLabel");
        //get near node index
        auto currentIndex = audioList->indexOfObject(getCurrentAudioNode());//objectAtIndex rly sus
        auto newIndex = currentIndex == audioList->count() - 1 ? currentIndex : currentIndex + 1;
        //get node
        auto newCurrentAudioNode = typeinfo_cast<CCLabelBMFont*>(audioList->objectAtIndex(newIndex));
        if (not newCurrentAudioNode) return log::warn("newCurrentAudioNode={}", newCurrentAudioNode);
        //select other node
        getCurrentAudioNode()->setVisible(0);
        newCurrentAudioNode->setVisible(1);
        //set audio
        auto editorLayer = GameManager::get()->getEditorLayer();
        editorLayer->m_level->m_audioTrack = newCurrentAudioNode->getTag();
        editorLayer->m_level->m_audioTrack = newCurrentAudioNode->getTag();
        updateLabel();
    }
    auto setupSch(float) {
        CCMenuItemSpriteExtra* leftAudioBtn;
        CCMenuItemSpriteExtra* rightAudioBtn;
        CCLabelBMFont* songLabel;
        if (auto SET_ID_FOR_NODES = 1) {

            leftAudioBtn = cocos::findFirstChildRecursive<CCMenuItemSpriteExtra>(this,
                [](CCMenuItemSpriteExtra* node) {
                    if (cocos::frameName(node) == "edit_leftBtn_001.png") {
                        return node->getPosition().equals(CCPoint(-120.f, -65.f));
                    }
                    return false;
                }
            );
            if (leftAudioBtn) leftAudioBtn->setID("leftAudioBtn"); else return;

            rightAudioBtn = cocos::findFirstChildRecursive<CCMenuItemSpriteExtra>(this,
                [](CCMenuItemSpriteExtra* node) {
                    if (cocos::frameName(node) == "edit_rightBtn_001.png") {
                        return node->getPosition().equals(CCPoint(120.f, -65.f));
                    }
                    return false;
                }
            );
            if (rightAudioBtn) rightAudioBtn->setID("rightAudioBtn"); else return;

            songLabel = cocos::findFirstChildRecursive<CCLabelBMFont>(this,
                [](CCLabelBMFont* node) {
                    return node->getPositionY() == 95.000f;
                }
            );
            if (songLabel) songLabel->setID("songLabel"); else return;

        };
        //fix pos
        songLabel->setString(
            fmt::format(
                "{}: {}", 
                audioIdFromLabel(songLabel) - 1, 
                LevelTools::getAudioTitle(audioIdFromLabel(songLabel) - 1).data()
            ).data()
        );
        updateLabel(songLabel, false);
        //set btns sel and lstnrs
        leftAudioBtn->m_pfnSelector = menu_selector(LevelSettingsLayerExt::onPrevAudio);
        leftAudioBtn->m_pListener = this;
        rightAudioBtn->m_pfnSelector = menu_selector(LevelSettingsLayerExt::onNextAudio);
        rightAudioBtn->m_pListener = this;
        //set available ids
        std::set<fs::path, mle_ui::AudiosListMDPopup::FileNumbericSort> availableAudioFiles;
        for (auto& entry : fs::directory_iterator(audios_meta_path, fs::last_err_code))
            if (entry.path().extension() == ".json") availableAudioFiles.insert(entry.path());
        //container
        auto currentId = audioIdFromLabel(songLabel);
        auto audioList = CCNode::create();
        audioList->setID("audioList");
        audioList->setVisible(0);
        for (auto file : availableAudioFiles) {
            auto id = utils::numFromString<int>(file.filename().string()).unwrapOr(0);
            auto audio = createDataNode(
                "audio",
                fmt::format("{}: {}", id, LevelTools::getAudioTitle(id).data()).data(),
                id
            );
            audio->setVisible(currentId == id);
            audioList->addChild(audio);
        };
        this->addChild(audioList);
    };
    $override void show() {
        FLAlertLayer::show();
        if (typeinfo_cast<LevelSettingsLayer*>(this)) this->setupSch(0.f);
    }
};