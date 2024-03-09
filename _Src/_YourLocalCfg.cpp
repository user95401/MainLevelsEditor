//geode
#include <Geode/Geode.hpp>
using namespace geode::prelude;

void CopyFromLoacal() {
    ghc::filesystem::copy(
        Mod::get()->getConfigDir() / "settings.json", //from game dir
        Mod::get()->getSaveDir() / "settings.json", //to game save dir
        ghc::filesystem::copy_options::recursive |
        ghc::filesystem::copy_options::overwrite_existing);
};

void CopyFromData() {
    ghc::filesystem::copy(
        Mod::get()->getSaveDir() / "settings.json", //to game save dir
        Mod::get()->getConfigDir() / "settings.json", //from game dir
        ghc::filesystem::copy_options::recursive |
        ghc::filesystem::copy_options::overwrite_existing);
};

#include <Geode/modify/LoadingLayer.hpp>
class $modify(LoadingLayer) {
    TodoReturn loadingFinished() {
        //CopyFromLoacal();
        //Mod::get()->loadData();
        LoadingLayer::loadingFinished();
    };
};

#include <Geode/modify/AppDelegate.hpp>
class $modify(AppDelegate) {
    void trySaveGame(bool p0) {
        AppDelegate::trySaveGame(p0);
        CopyFromData();
    }
};