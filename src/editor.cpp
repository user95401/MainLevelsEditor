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
                "<cr>User coins</c> will be <cy>saved as Secret Coins</c>!\n"
                " "
                , "chatFont.fnt", 1.f, 600.f, { 0.5f, 0.5f }, 14.f, 0
            ), -1, 6302);
            node->updateLayout();
        }
        return rtn;
    }
};