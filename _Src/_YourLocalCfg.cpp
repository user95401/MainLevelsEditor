//geode
#include <Geode/Geode.hpp>
using namespace geode::prelude;

void CopyFromLoacal() {
    try {
        ghc::filesystem::copy(
            Mod::get()->getConfigDir(), //from game dir
            Mod::get()->getSaveDir(), //to game save dir
            ghc::filesystem::copy_options::recursive |
            ghc::filesystem::copy_options::overwrite_existing);
    }
    catch (std::exception& theException) {
        log::error("{}", theException.what());
    }
}

void CopyFromData() {
    try {
        ghc::filesystem::copy(
            Mod::get()->getSaveDir(), //to game save dir
            Mod::get()->getConfigDir(), //from game dir
            ghc::filesystem::copy_options::recursive | 
            ghc::filesystem::copy_options::overwrite_existing);
    }
    catch (std::exception& theException) {
        log::error("{}", theException.what());
    }
}

$on_mod(Loaded) {
    CopyFromLoacal();
    Mod::get()->loadData();
};

#include <Geode/modify/AppDelegate.hpp>
class $modify(AppDelegate) {
    void trySaveGame(bool p0) {
        AppDelegate::trySaveGame(p0);
        CopyFromData();
    }
};