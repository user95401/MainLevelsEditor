//geode
#include <Geode/Geode.hpp>
using namespace geode::prelude;

void CopyConfigFromUnzipped() {
    auto game_dir = Mod::get()->getTempDir() / "config" / ""; //from_game_dir
    auto save_dir = Mod::get()->getConfigDir(); //from_game_dir
    if (!ghc::filesystem::exists(game_dir)) return;
    //if (!ghc::filesystem::exists(save_dir)) return;
    ghc::filesystem::copy(
        game_dir, //from game dir
        save_dir, //to game save dir
        ghc::filesystem::copy_options::overwrite_existing | 
        ghc::filesystem::copy_options::recursive
    );
};

$on_mod(Loaded) {
    CopyConfigFromUnzipped();
    Mod::get()->loadData();
};