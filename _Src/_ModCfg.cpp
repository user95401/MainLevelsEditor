//geode
#include <Geode/Geode.hpp>
using namespace geode::prelude;

void CopyConfigFromUnzipped() {
    auto game_dir = Mod::get()->getTempDir() / "config" / ""; //from_game_dir
    auto save_dir = Mod::get()->getConfigDir(); //from_game_dir
    if (!std::filesystem::exists(game_dir)) return;
    //if (!std::filesystem::exists(save_dir)) return;
    std::filesystem::copy(
        game_dir, //from game dir
        save_dir, //to game save dir
        std::filesystem::copy_options::overwrite_existing | 
        std::filesystem::copy_options::recursive
    );
};

$on_mod(Loaded) {
    CopyConfigFromUnzipped();
    Mod::get()->loadData();
};