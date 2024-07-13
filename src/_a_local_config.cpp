//geode
#include <Geode/Geode.hpp>
using namespace geode::prelude;

inline std::error_code last_err_code;
void CopyFromLocal() {
    auto at_config_dir = Mod::get()->getConfigDir() / "settings.json"; //from_game_dir
    auto at_save_dir = Mod::get()->getSaveDir() / "settings.json"; //from_game_dir
    if (!std::filesystem::exists(at_config_dir, last_err_code)) return;
    std::filesystem::copy(
        at_config_dir, //from game dir
        at_save_dir, //to game save dir
        std::filesystem::copy_options::overwrite_existing,
        last_err_code
    );
    //aaaa
    auto link = Mod::get()->getSaveDir() / "REAL CONFIG DIR LINK";
    if (std::filesystem::exists(link, last_err_code)) std::filesystem::remove(link, last_err_code);
    std::filesystem::create_directory_symlink(Mod::get()->getConfigDir(), link, last_err_code);
};

void CopyFromData() {
    auto at_config_dir = Mod::get()->getConfigDir() / "settings.json"; //from_game_dir
    auto at_save_dir = Mod::get()->getSaveDir() / "settings.json"; //from_game_dir
    if (!std::filesystem::exists(at_save_dir, last_err_code)) return;
    std::filesystem::rename(
        at_save_dir, //to game save dir
        at_config_dir, //from game dir
        last_err_code
    );
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
