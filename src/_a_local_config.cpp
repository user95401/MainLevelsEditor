//geode
#include <Geode/Geode.hpp>
using namespace geode::prelude;

void CopyFromLocal() {
    auto game_dir = Mod::get()->getConfigDir() / "settings.json"; //from_game_dir
    auto save_dir = Mod::get()->getSaveDir() / "settings.json"; //from_game_dir
    if (!std::filesystem::exists(game_dir)) return;
    //if (!std::filesystem::exists(save_dir)) return;
    std::filesystem::copy(
        game_dir, //from game dir
        save_dir, //to game save dir
        std::filesystem::copy_options::overwrite_existing);
    //aaaa
    auto link = Mod::get()->getSaveDir() / "REAL CONFIG DIR LINK";
    if (std::filesystem::exists(link)) std::filesystem::remove(link);
    std::filesystem::create_directory_symlink(Mod::get()->getConfigDir(), link);
};

void CopyFromData() {
    auto game_dir = Mod::get()->getConfigDir() / "settings.json"; //from_game_dir
    auto save_dir = Mod::get()->getSaveDir() / "settings.json"; //from_game_dir
    //if (!std::filesystem::exists(game_dir)) return;
    if (!std::filesystem::exists(save_dir)) return;
    std::filesystem::copy(
        save_dir, //to game save dir
        game_dir, //from game dir
        std::filesystem::copy_options::overwrite_existing);
};

$on_mod(Loaded) {
    CopyFromLocal();
    Mod::get()->loadData();
};

#include <Geode/modify/AppDelegate.hpp>
class $modify(AppDelegate) {
    void trySaveGame(bool p0) {
        AppDelegate::trySaveGame(p0);
        CopyFromData();
    }
};