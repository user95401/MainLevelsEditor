#include "_main.hpp"

void Loaded() {
	//dirs
	fs::create_directories(levels_meta_path);
	//add info
	std::ofstream(levels_meta_path / "_here is {id}.json meta files about names, stars and stuff") << "asd";
	std::ofstream(levels_path / "_here u put {id}.txt level data files");
	//preset all org levels
	for (int i = 1; i <= 100; i++) {
		auto level_in_game_path = CCFileUtils::sharedFileUtils()->fullPathForFilename(fmt::format("levels/{}.txt", i).c_str(), 0);
		auto level_path_to_save = (levels_path / fmt::format("{}.txt", i));
		auto level_is_exists_in_gd = cocos::fileExistsInSearchPaths(level_in_game_path.c_str());
		auto level_is_exists_in_mod = cocos::fileExistsInSearchPaths(level_path_to_save.string().c_str());
		if (level_is_exists_in_gd and not level_is_exists_in_mod) {
			log::debug("ofstreamingo...");
			std::ofstream(level_path_to_save) 
				<< read_file(level_in_game_path);
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
		if (level->m_levelType == GJLevelType::Local)
			level->m_levelString = GameLevelManager::get()->getMainLevel(level->m_levelID.value(), 0)->m_levelString.c_str();
		auto oldID = level->m_levelID.value();
		level->m_levelID = 999; //temp "Load Failed" bypass
		auto rtn = PlayLayer::init(level, useReplay, dontCreateObjects);
		level->m_levelID = oldID;
		return rtn;
	}
};

#include <Geode/modify/GameLevelManager.hpp>
class $modify(GameLevelManagerExt, GameLevelManager) {
	static void updateLevelByJson(GJGameLevel* level) {
		auto level_meta_file = levels_meta_path / fmt::format("{}.json", level->m_levelID.value());
		auto file_content = read_file(level_meta_file.string());
		//json val
		auto value = matjson::parse("{}");
		//file parse
		auto error = std::string();
		auto parse = matjson::parse(file_content, error);
		if (parse.has_value()) value = parse.value();
		//setup level
		level->m_levelName = value.try_get<std::string>("m_levelName").value_or(level->m_levelName.data());
		level->m_levelDesc = value.try_get<std::string>("m_levelDesc").value_or(level->m_levelDesc.data());
		level->m_creatorName = value.try_get<std::string>("m_creatorName").value_or(level->m_creatorName.data());
		level->m_difficulty = (GJDifficulty)value.try_get<int>("m_difficulty").value_or((int)level->m_difficulty);
		level->m_stars = value.try_get<int>("m_stars").value_or(level->m_stars.value());
		level->m_audioTrack = value.try_get<int>("m_audioTrack").value_or(level->m_audioTrack);
		level->m_songID = value.try_get<int>("m_songID").value_or(level->m_songID);
		level->m_levelVersion = value.try_get<int>("m_levelVersion").value_or(level->m_levelVersion);
		level->m_gameVersion = value.try_get<int>("m_gameVersion").value_or(level->m_gameVersion);
		level->m_levelType = (GJLevelType)value.try_get<int>("m_levelType").value_or((int)level->m_levelType);
		level->m_capacityString = value.try_get<std::string>("m_capacityString").value_or(level->m_capacityString.data());
		level->m_songIDs = value.try_get<std::string>("m_songIDs").value_or(level->m_songIDs.data());
		level->m_sfxIDs = value.try_get<std::string>("m_sfxIDs").value_or(level->m_sfxIDs.data());
		//setup json
		value.try_set("________MAIN_STUFF________", " vvv vvv ");
		value.try_set("m_levelName", std::string(level->m_levelName.data()));
		value.try_set("m_audioTrack", level->m_audioTrack);
		value.try_set("m_difficulty", (int)level->m_difficulty);
		value.try_set("m_stars", level->m_stars.value());
		value.try_set("________SOME_SHIT________", " vvv vvv ");
		value.try_set("m_levelDesc", std::string(level->m_levelDesc.data()));
		value.try_set("m_creatorName", std::string(level->m_creatorName.data()));;
		value.try_set("m_songID", level->m_songID);
		value.try_set("m_levelVersion", level->m_levelVersion);
		value.try_set("m_gameVersion", level->m_gameVersion);
		value.try_set("m_levelType", (int)level->m_levelType);
		value.try_set("m_capacityString", std::string(level->m_capacityString.data()));
		value.try_set("m_songIDs", std::string(level->m_songIDs.data()));
		value.try_set("m_sfxIDs", std::string(level->m_sfxIDs.data()));
		//save json
		std::ofstream(level_meta_file) << value.dump(matjson::TAB_INDENTATION);
	}
	GJGameLevel* getMainLevel(int levelID, bool dontGetLevelString) {
		auto level = GameLevelManager::getMainLevel(levelID, dontGetLevelString);
		//json meta
		updateLevelByJson(level);
		//lvl str
		auto toRead = std::string();
		auto dataFile = std::string(CCFileUtils::get()->fullPathForFilename(fmt::format("levels/{}.txt", levelID).c_str(), 1).data());
		auto dataFileAtMod = (levels_path / fmt::format("{}.txt", levelID)).string();
		log::debug("dataFile: {}", dataFile);
		log::debug("dataFileAtMod: {}", dataFileAtMod);
		if (cocos::fileExistsInSearchPaths(dataFileAtMod.c_str())) toRead = dataFileAtMod;
		else toRead = dataFile;
		auto fileContent = read_file(toRead);
		level->m_levelString = fileContent;
		return level;
	}
};

#include <Geode/modify/LevelTools.hpp>
class $modify(LevelTools) {
	static bool NOT_NOW___verifyLevelIntegrity(gd::string p0, int p1) {
		//hooking error lol
		return 1;
	}
};
