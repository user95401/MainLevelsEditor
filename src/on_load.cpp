#include "_main.hpp"
#include "_updater.hpp"

void Loaded() {
	//dirs
	fs::create_directories(levels_meta_path);
	fs::create_directories(audios_meta_path);
	fs::create_directories(artists_meta_path);
	//add info
	std::ofstream(levels_meta_path / "_here is {id}.json meta files about names, stars and stuff") << "asd";
	std::ofstream(levels_path / "_here u put {id}.txt level data files");
	std::ofstream(audios_meta_path / "_here is {id}.json meta files about name, file path etc") << "asd";
	std::ofstream(artists_meta_path / "_here is {id}.json meta files about artists") << "asd";
	std::ofstream(songs_path / "_here u put {id}.mp3 or {id}.ogg music files");
	//preset all org levels
	for (int i = 1; i <= 100; i++) {
		std::string level_in_game_path = CCFileUtils::sharedFileUtils()->fullPathForFilename(fmt::format("levels/{}.txt", i).c_str(), 0);
		auto level_path_to_save = (levels_path / fmt::format("{}.txt", i));
		auto level_is_exists_in_gd = cocos::fileExistsInSearchPaths(level_in_game_path.c_str());
		auto level_is_exists_in_mod = cocos::fileExistsInSearchPaths(level_path_to_save.string().c_str());
		if (level_is_exists_in_gd and not level_is_exists_in_mod) {
			std::ofstream(level_path_to_save)
				<< fs::read(level_in_game_path);
		}
	}
	//preset all org songs
	for (int i = -10; i <= 100; i++) {
		auto huh = mle_audiotools::Audio(i, false);
	}
	//preset all org artists
	for (int i = -10; i <= 100; i++) {
		auto huh = mle_audiotools::Artist(i, false);
	}
	//add sp ea
	CCFileUtils::get()->addPriorityPath(Mod::get()->getConfigDir().string().c_str());
	CCFileUtils::get()->addPriorityPath(songs_path.string().c_str());
};
#include <Geode/modify/LoadingLayer.hpp>
class $modify(LoadingLayer) {
	$override void loadAssets() {
		if (m_loadStep == 0) Loaded();
		LoadingLayer::loadAssets();
	}
};