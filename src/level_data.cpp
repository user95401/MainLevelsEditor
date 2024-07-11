#include "_main.hpp"

void Loaded() {
	//dirs
	my_fs::create_directories(levels_meta_path);
	//add info
	std::ofstream(levels_meta_path / "_here is {id}.json meta files about names, stars and stuff") << "asd";
	std::ofstream(levels_path / "_here u put {id}.txt level data files");
	//preset all org levels
	for (int i = 1; i <= 100; i++) {
		std::string level_in_game_path = CCFileUtils::sharedFileUtils()->fullPathForFilename(fmt::format("levels/{}.txt", i).c_str(), 0);
		auto level_path_to_save = (levels_path / fmt::format("{}.txt", i));
		auto level_is_exists_in_gd = cocos::fileExistsInSearchPaths(level_in_game_path.c_str());
		auto level_is_exists_in_mod = cocos::fileExistsInSearchPaths(level_path_to_save.string().c_str());
		if (level_is_exists_in_gd and not level_is_exists_in_mod) {
			log::debug("ofstreamingo...");
			std::ofstream(level_path_to_save) 
				<< my_fs::read(level_in_game_path);
		}
		//add sp ea
		CCFileUtils::get()->addPriorityPath(Mod::get()->getConfigDir().string().c_str());
	}
};
#include <Geode/modify/LoadingLayer.hpp>
class $modify(LoadingLayer) {
	void loadAssets() {
		if (m_loadStep == 0) Loaded();
		LoadingLayer::loadAssets();
	}
};

//huh
#include <Geode/modify/PlayLayer.hpp>
class $modify(PlayLayer) {
	bool init(GJGameLevel * level, bool useReplay, bool dontCreateObjects) {
		auto oldID = level->m_levelID.value();
		level->m_levelID = 999; //temp "Load Failed" bypass
		auto rtn = PlayLayer::init(level, useReplay, dontCreateObjects);
		level->m_levelID = oldID;
		return rtn;
	}
};

#include <Geode/modify/LocalLevelManager.hpp>
class $modify(LocalLevelManagerExt, LocalLevelManager) {
	gd::string getMainLevelString(int p0) {
		//LocalLevelManager::getMainLevelString(p0);
		auto levelID = p0;
		//get data file path
		auto toRead = std::string();
		auto dataFile = std::string(CCFileUtils::get()->fullPathForFilename(fmt::format("levels/{}.txt", levelID).c_str(), 0).data());
		auto dataFileAtMod = (levels_path / fmt::format("{}.txt", levelID)).string();
		if (my_fs::exists(dataFileAtMod)) toRead = dataFileAtMod;
		else toRead = dataFile;
		//read
		auto fileContent = my_fs::read(toRead);
		//return
		return gd::string(fileContent.c_str());
	}
};

//// fffffffffffffuc a here is aaa problematic functions idk lol

#include <Geode/modify/LevelTools.hpp>
class $modify(LevelTools) {
	static void updateLevelByJson(GJGameLevel * level) {
		auto level_meta_file = levels_meta_path / fmt::format("{}.json", level->m_levelID.value());
		auto file_content = my_fs::read(level_meta_file);
		//json val
		auto value = matjson::parse("{}");
		//file parse
		auto error = std::string();
		auto parse = matjson::parse(file_content, error);
		if (parse.has_value()) value = parse.value();
		//setup level
		levelFromJson(parse, level);
		//setup json
		value = jsonFromLevel(level);
		//save json
		std::ofstream(level_meta_file) << value.dump(matjson::TAB_INDENTATION);
	}
	static GJGameLevel* getLevel(int levelID, bool dontGetLevelString) {
		auto level = LevelTools::getLevel(levelID, dontGetLevelString);
		//json meta
		updateLevelByJson(level);
		return level;
	};
	static bool NOT_NOW___verifyLevelIntegrity(gd::string p0, int p1) {
		//hooking error lol
		return 1;
	}
};