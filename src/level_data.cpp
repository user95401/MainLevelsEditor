#include "_main.hpp"
using namespace mle_leveltools;
//huh
#include <Geode/modify/PlayLayer.hpp>
class $modify(PlayLayer) {
	$override bool init(GJGameLevel * level, bool useReplay, bool dontCreateObjects) {
		auto oldID = level->m_levelID.value();
		level->m_levelID = 999; //temp "Load Failed" bypass
		auto rtn = PlayLayer::init(level, useReplay, dontCreateObjects);
		level->m_levelID = oldID;
		return rtn;
	}
};

#include <Geode/modify/LocalLevelManager.hpp>
class $modify(LocalLevelManagerExt, LocalLevelManager) {
	$override gd::string getMainLevelString(int p0) {
		log::debug("{}({})", __func__, p0);
		//LocalLevelManager::getMainLevelString(p0);
		auto levelID = p0;
		//get data file path
		auto toRead = std::string();
		auto dataFile = std::string(CCFileUtils::get()->fullPathForFilename(fmt::format("levels/{}.txt", levelID).c_str(), 0).data());
		auto dataFileAtMod = (levels_path / fmt::format("{}.txt", levelID)).string();
		if (fs::exists(dataFileAtMod)) toRead = dataFileAtMod;
		else toRead = dataFile;
		//read
		auto fileContent = fs::read(toRead);
		//return
		return gd::string(fileContent.c_str());
	}
};

//// fffffffffffffuc a here is aaa problematic functions idk lol

#include <Geode/modify/LevelTools.hpp>
class $modify(LevelTools) {
	$override static GJGameLevel* getLevel(int levelID, bool dontGetLevelString) {
		auto level = LevelTools::getLevel(levelID, 1);
		if (not dontGetLevelString) {
			level->m_levelString = LocalLevelManager::get()->getMainLevelString(levelID);
		}
		level->m_levelID = levelID;
		//json meta
		updateLevelByJson(level);
		return level;
	};
	$override static bool NOT_NOW___verifyLevelIntegrity(gd::string p0, int p1) {
		//hooking error lol
		return 1;
	}
};