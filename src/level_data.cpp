#include "_main.hpp"
using namespace mle_leveltools;

//huh
#include <Geode/modify/PlayLayer.hpp>
class $modify(PlayLayer) {
	$override void resetLevel() {
		//new ones dont saved by game idk why
		auto lm = GameLevelManager::sharedState();
		lm->m_mainLevels->setObject(m_level, lm->getLevelKey(m_level->m_levelID.value()));
		return PlayLayer::resetLevel();
	};
};

#include <Geode/modify/LocalLevelManager.hpp>
class $modify(LocalLevelManagerExt, LocalLevelManager) {
	$override gd::string getMainLevelString(int p0) {
		//log::debug("{}({})", __func__, p0);
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

#include <Geode/modify/LevelTools.hpp>
class $modify(LevelTools) {
	$override static GJGameLevel* getLevel(int levelID, bool dontGetLevelString) {
		auto level = LevelTools::getLevel(levelID, 1);
		if (not dontGetLevelString) {
			level->m_levelString = LocalLevelManager::get()->getMainLevelString(levelID);
		}
		level->m_levelID = levelID;
		//progress
		auto manager = GameLevelManager::sharedState();
		if (GJGameLevel* lvlFromDict = (GJGameLevel*)manager->m_mainLevels->objectForKey(manager->getLevelKey(levelID))) {
			level->m_attempts = lvlFromDict->m_attempts;
			level->m_jumps = lvlFromDict->m_jumps;
			level->m_clicks = lvlFromDict->m_clicks;
			level->m_attemptTime = lvlFromDict->m_attemptTime;
			level->m_chk = lvlFromDict->m_chk;
			level->m_isChkValid = lvlFromDict->m_isChkValid;
			level->m_isCompletionLegitimate = lvlFromDict->m_isCompletionLegitimate;
			level->m_normalPercent = lvlFromDict->m_normalPercent;
			level->m_orbCompletion = lvlFromDict->m_orbCompletion;
			level->m_newNormalPercent2 = lvlFromDict->m_newNormalPercent2;
			level->m_practicePercent = lvlFromDict->m_practicePercent;
			level->m_bestTime = lvlFromDict->m_bestTime;
			level->m_bestPoints = lvlFromDict->m_bestPoints;
			level->m_k111 = lvlFromDict->m_k111;
		};
		//json meta
		updateLevelByJson(level);
		return level;
	};
	$override static bool verifyLevelIntegrity(gd::string p0, int p1) {
		LevelTools::verifyLevelIntegrity(p0, p1);
		return 1;
	}
};