#include "_main.hpp"

#include <Geode/modify/EditorPauseLayer.hpp>
class $modify(EditorPauseLayerExt, EditorPauseLayer) {
    $override void saveLevel() {
        EditorPauseLayer::saveLevel();
        if (not this->m_editorLayer->getChildByIDRecursive("IsMainLevelEditor")) return;
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
    struct Fields {
        SEL_MenuHandler m_orgEditObjectSpecialSel;
    };
    void callEditObjectSpecial() {
        if (auto editSpecialButton = typeinfo_cast<CCMenuItemSpriteExtra*>(this->getChildByIDRecursive("edit-special-button"))) {
            editSpecialButton->m_pfnSelector = m_fields->m_orgEditObjectSpecialSel;
            editSpecialButton->activate();
            editSpecialButton->m_pfnSelector = menu_selector(EditorUIExt::customEditObjectSpecial);
        }
    }
    void customEditObjectSpecial(CCObject* p0) {
        if (not this->m_selectedObject) return;
        if (not this->m_editorLayer->getChildByIDRecursive("IsMainLevelEditor")) {
            return callEditObjectSpecial();
        }
        //user coin id 1329, 142 secret coin
        if (this->m_selectedObject->m_objectID == 1329) {
            this->m_selectedObject->m_objectID = 142;
            this->editObject(p0);
            this->m_selectedObject->m_objectID = 1329;
            return;
        }
    };
    bool init(LevelEditorLayer* editorLayer) {
        auto rtn = EditorUI::init(editorLayer);

        //wasnt bindings for that yet
        if (auto editSpecialButton = typeinfo_cast<CCMenuItemSpriteExtra*>(this->getChildByIDRecursive("edit-special-button"))) {
            m_fields->m_orgEditObjectSpecialSel = editSpecialButton->m_pfnSelector;
            editSpecialButton->m_pfnSelector = menu_selector(EditorUIExt::customEditObjectSpecial);
        }

        return rtn;
    }
};