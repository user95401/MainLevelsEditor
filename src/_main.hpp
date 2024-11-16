#pragma once
#include <Geode/Geode.hpp>
#include <Geode/ui/GeodeUI.hpp>
using namespace geode::prelude;

#include <regex>

//lol
#define SETTING(type, key_name) Mod::get()->getSettingValue<type>(key_name)

#define MEMBER_BY_OFFSET(type, class, offset) *reinterpret_cast<type*>(reinterpret_cast<uintptr_t>(class) + offset)
template<typename T, typename U> constexpr size_t OFFSET_BY_MEMBER(U T::* member) { return (char*)&((T*)nullptr->*member) - (char*)nullptr; }

inline auto levels_path = Mod::get()->getConfigDir() / "levels";
inline auto levels_meta_path = levels_path / "_meta-v2";
inline auto songs_path = Mod::get()->getConfigDir() / "songs";
inline auto audios_meta_path = songs_path / "_audios";
inline auto artists_meta_path = songs_path / "_artists";
#include <_fs.hpp>

#define public_cast(value, member) [](auto* v) { \
	class FriendClass__; \
	using T = std::remove_pointer<decltype(v)>::type; \
	class FriendeeClass__: public T { \
	protected: \
		friend FriendClass__; \
	}; \
	class FriendClass__ { \
	public: \
		auto& get(FriendeeClass__* v) { return v->member; } \
	} c; \
	return c.get(reinterpret_cast<FriendeeClass__*>(v)); \
}(value)

namespace geode::cocos {
    inline std::string frameName(CCNode* node) {
        if (node == nullptr) return "NIL_NODE";
        if (auto textureProtocol = dynamic_cast<CCTextureProtocol*>(node)) {
            if (auto texture = textureProtocol->getTexture()) {
                if (auto spriteNode = dynamic_cast<CCSprite*>(node)) {
                    auto* cachedFrames = CCSpriteFrameCache::sharedSpriteFrameCache()->m_pSpriteFrames;
                    const auto rect = spriteNode->getTextureRect();
                    for (auto [key, frame] : CCDictionaryExt<std::string, CCSpriteFrame*>(cachedFrames)) {
                        if (frame->getTexture() == texture && frame->getRect() == rect) {
                            return key.c_str();
                        }
                    }
                }
                auto* cachedTextures = CCTextureCache::sharedTextureCache()->m_pTextures;
                for (auto [key, obj] : CCDictionaryExt<std::string, CCTexture2D*>(cachedTextures)) {
                    if (obj == texture) {
                        return key.c_str();
                    }
                }
            }
        }
        auto btnSpriteTry = frameName(getChild(node, 0));
        if (
            btnSpriteTry != "NIL_NODE"
            and btnSpriteTry != "CANT_GET_FRAME_NAME"
            ) return btnSpriteTry;
        return "CANT_GET_FRAME_NAME";
    }
    inline auto createDataNode(std::string id, std::string text = "", int tag = 0) {
        auto node = CCLabelBMFont::create("", "chatFont.fnt");
        node->setID(id);
        node->setString(text.c_str());
        if (tag != 0) node->setTag(tag);
        node->setVisible(0);
        return node;
    }
    inline auto findDataNode(CCNode* parent, std::string id) {
        auto node = typeinfo_cast<CCLabelBMFont*>(parent->getChildByIDRecursive(id));
        if (node) log::warn("FAILED TO FIND DATA NODE! id: {}", id);
        return node;
    }
};

namespace matjson {
    inline auto parse(std::string file_content, std::string& error) {
        auto parse = matjson::parse(file_content);
        if (parse.isErr()) error == parse.unwrapErr().message;
        return parse;
    };
}

namespace geode::utils::string {
    inline std::vector<std::string> explode(std::string separator, std::string input) {
        std::vector<std::string> vec;
        for (int i{ 0 }; i < input.length(); ++i) {
            int pos = input.find(separator, i);
            if (pos < 0) { vec.push_back(input.substr(i)); break; }
            int count = pos - i;
            vec.push_back(input.substr(i, count));
            i = pos + separator.length() - 1;
        }
        if (vec.size() == 0) vec.push_back(input);
        return vec;
    }
}

namespace mle_leveltools {
    inline GJGameLevel* levelFromJson(matjson::Value value, GJGameLevel* levelToRewrite = nullptr) {
        GJGameLevel* level = levelToRewrite ? levelToRewrite : LevelTools::getLevel(1, 0);

        auto try_get_main = value["main"];
        if (try_get_main.isObject()) {
            auto main = try_get_main;
            level->m_levelName = main["m_levelName"].asString().unwrapOr(level->m_levelName.data());
            level->m_audioTrack = main["m_audioTrack"].asInt().unwrapOr(level->m_audioTrack);
            level->m_difficulty = (GJDifficulty)main["m_difficulty"].asInt().unwrapOr((int)level->m_difficulty);
            level->m_stars = main["m_stars"].asInt().unwrapOr(level->m_stars.value());
            level->m_requiredCoins = main["m_requiredCoins"].asInt().unwrapOr(level->m_requiredCoins);
            level->m_levelLength = main["m_levelLength"].asInt().unwrapOr(level->m_levelLength);
        }

        auto try_get_other = value["other"];
        if (try_get_other.isObject()) {
            auto other = try_get_other;
            level->m_levelDesc = value["m_levelDesc"].asString().unwrapOr(level->m_levelDesc.data());
            level->m_creatorName = value["m_creatorName"].asString().unwrapOr(level->m_creatorName.data());
            level->m_songID = value["m_songID"].asInt().unwrapOr(level->m_songID);
            level->m_levelVersion = value["m_levelVersion"].asInt().unwrapOr(level->m_levelVersion);
            level->m_gameVersion = value["m_gameVersion"].asInt().unwrapOr(level->m_gameVersion);
            level->m_levelType = (GJLevelType)value["m_levelType"].asInt().unwrapOr((int)level->m_levelType);
            level->m_capacityString = value["m_capacityString"].asString().unwrapOr(level->m_capacityString.data());
            level->m_songIDs = value["m_songIDs"].asString().unwrapOr(level->m_songIDs.data());
            level->m_sfxIDs = value["m_sfxIDs"].asString().unwrapOr(level->m_sfxIDs.data());
        }

        return level;
    }
    inline auto jsonFromLevel(GJGameLevel* level) {
        auto value = matjson::Value();

        auto main = matjson::Value();
        main.set("m_levelName", std::string(level->m_levelName.data()));
        main.set("m_audioTrack", level->m_audioTrack);
        main.set("m_difficulty", (int)level->m_difficulty);
        main.set("m_stars", level->m_stars.value());
        main.set("m_requiredCoins", level->m_requiredCoins);
        main.set("m_levelLength", level->m_levelLength);
        value.set("main", main);

        auto other = matjson::Value();
        other.set("m_levelDesc", std::string(level->m_levelDesc.data()));
        other.set("m_creatorName", std::string(level->m_creatorName.data()));;
        other.set("m_songID", level->m_songID);
        other.set("m_levelVersion", level->m_levelVersion);
        other.set("m_gameVersion", level->m_gameVersion);
        other.set("m_levelType", (int)level->m_levelType);
        other.set("m_capacityString", std::string(level->m_capacityString.data()));
        other.set("m_songIDs", std::string(level->m_songIDs.data()));
        other.set("m_sfxIDs", std::string(level->m_sfxIDs.data()));
        value.set("other", other);

        return value;
    }
    inline void updateLevelByJson(GJGameLevel* level) {
        auto level_meta_file = levels_meta_path / fmt::format("{}.json", level->m_levelID.value());
        auto file_content = fs::read(level_meta_file);
        //json val
        auto value = matjson::parse("{}").unwrapOrDefault();
        //file parse
        auto error = std::string();
        auto parse = matjson::parse(file_content, error);
        if (parse.isOk()) value = parse.unwrapOrDefault();
        //setup level
        levelFromJson(value, level);
        //setup json
        value = jsonFromLevel(level);
        //save json
        std::ofstream(level_meta_file) << value.dump(matjson::TAB_INDENTATION);
    }
    inline auto replaceSecretCoinsByUserOnesInLevel(GJGameLevel* level) {
#ifdef DEBUG
        GEODE_WINDOWS(
            CCMessageBox(
                "U USE DEBUG VER!\nCOINS WILL NOT BE REPLACED!",
                Mod::get()->getName().c_str()
            )
        );
        return level;
#endif // DEBUG
        //user coin id 1329, 142 secret coin
        auto replTar = ",142,";
        auto replBy = ",1329,";
        gd::string decompressed = ZipUtils::decompressString(level->m_levelString, false, 0);
        decompressed = std::regex_replace(decompressed.data(), std::regex(replTar), replBy).c_str();
        level->m_levelString = ZipUtils::compressString(decompressed, false, 0);
        return level;
    }
    inline auto replaceUserCoinsBySecretOnesInLevel(GJGameLevel* level) {
#ifdef DEBUG
        GEODE_WINDOWS(
            CCMessageBox(
                "U USE DEBUG VER!\nCOINS WILL NOT BE REPLACED!",
                Mod::get()->getName().c_str()
            )
        );
        return level;
#endif // DEBUG
        //user coin id 1329, 142 secret coin
        auto replTar = ",1329,";
        auto replBy = ",142,";
        gd::string decompressed = ZipUtils::decompressString(level->m_levelString, false, 0);
        decompressed = std::regex_replace(decompressed.data(), std::regex(replTar), replBy).c_str();
        level->m_levelString = ZipUtils::compressString(decompressed, false, 0);
        return level;
    }
    inline auto updateLevelDataAndMetaFiles(gd::string str, GJGameLevel* level) {
        level->m_levelString = str;
        replaceUserCoinsBySecretOnesInLevel(level);
        auto level_path_to_save = (levels_path / fmt::format("{}.txt", level->m_levelID.value()));
        std::ofstream(level_path_to_save) << std::string(level->m_levelString.data());
        auto level_meta_file = levels_meta_path / fmt::format("{}.json", level->m_levelID.value());
        std::ofstream(level_meta_file) << jsonFromLevel(level).dump(matjson::TAB_INDENTATION);
        return;
    }
    inline auto levelFromLevel(GJGameLevel* org_level) {
        auto new_level = GJGameLevel::create();
        new_level->m_lastBuildSave = org_level->m_lastBuildSave;
        new_level->m_levelID = org_level->m_levelID;
        new_level->m_levelName = org_level->m_levelName;
        new_level->m_levelDesc = org_level->m_levelDesc;
        new_level->m_levelString = org_level->m_levelString;
        new_level->m_creatorName = org_level->m_creatorName;
        new_level->m_recordString = org_level->m_recordString;
        new_level->m_uploadDate = org_level->m_uploadDate;
        new_level->m_updateDate = org_level->m_updateDate;
        new_level->m_unkString1 = org_level->m_unkString1;
        new_level->m_unkString2 = org_level->m_unkString2;
        new_level->m_unkPoint = org_level->m_unkPoint;
        new_level->m_userID = org_level->m_userID;
        new_level->m_accountID = org_level->m_accountID;
        new_level->m_difficulty = org_level->m_difficulty;
        new_level->m_audioTrack = org_level->m_audioTrack;
        new_level->m_songID = org_level->m_songID;
        new_level->m_levelRev = org_level->m_levelRev;
        new_level->m_unlisted = org_level->m_unlisted;
        new_level->m_friendsOnly = org_level->m_friendsOnly;
        new_level->m_objectCount = org_level->m_objectCount;
        new_level->m_levelIndex = org_level->m_levelIndex;
        new_level->m_ratings = org_level->m_ratings;
        new_level->m_ratingsSum = org_level->m_ratingsSum;
        new_level->m_downloads = org_level->m_downloads;
        new_level->m_isEditable = org_level->m_isEditable;
        new_level->m_gauntletLevel = org_level->m_gauntletLevel;
        new_level->m_gauntletLevel2 = org_level->m_gauntletLevel2;
        new_level->m_workingTime = org_level->m_workingTime;
        new_level->m_workingTime2 = org_level->m_workingTime2;
        new_level->m_lowDetailMode = org_level->m_lowDetailMode;
        new_level->m_lowDetailModeToggled = org_level->m_lowDetailModeToggled;
        new_level->m_disableShakeToggled = org_level->m_disableShakeToggled;
        new_level->m_selected = org_level->m_selected;
        new_level->m_localOrSaved = org_level->m_localOrSaved;
        new_level->m_disableShake = org_level->m_disableShake;
        new_level->m_isVerified = org_level->m_isVerified;
        new_level->m_isVerifiedRaw = org_level->m_isVerifiedRaw;
        new_level->m_isUploaded = org_level->m_isUploaded;
        new_level->m_hasBeenModified = org_level->m_hasBeenModified;
        new_level->m_levelVersion = org_level->m_levelVersion;
        new_level->m_gameVersion = org_level->m_gameVersion;
        new_level->m_attempts = org_level->m_attempts;
        new_level->m_jumps = org_level->m_jumps;
        new_level->m_clicks = org_level->m_clicks;
        new_level->m_attemptTime = org_level->m_attemptTime;
        new_level->m_chk = org_level->m_chk;
        new_level->m_isChkValid = org_level->m_isChkValid;
        new_level->m_isCompletionLegitimate = org_level->m_isCompletionLegitimate;
        new_level->m_normalPercent = org_level->m_normalPercent;
        new_level->m_orbCompletion = org_level->m_orbCompletion;
        new_level->m_newNormalPercent2 = org_level->m_newNormalPercent2;
        new_level->m_practicePercent = org_level->m_practicePercent;
        new_level->m_likes = org_level->m_likes;
        new_level->m_dislikes = org_level->m_dislikes;
        new_level->m_levelLength = org_level->m_levelLength;
        new_level->m_featured = org_level->m_featured;
        new_level->m_isEpic = org_level->m_isEpic;
        new_level->m_levelFavorited = org_level->m_levelFavorited;
        new_level->m_levelFolder = org_level->m_levelFolder;
        new_level->m_dailyID = org_level->m_dailyID;
        new_level->m_demon = org_level->m_demon;
        new_level->m_demonDifficulty = org_level->m_demonDifficulty;
        new_level->m_stars = org_level->m_stars;
        new_level->m_autoLevel = org_level->m_autoLevel;
        new_level->m_coins = org_level->m_coins;
        new_level->m_coinsVerified = org_level->m_coinsVerified;
        new_level->m_password = org_level->m_password;
        new_level->m_originalLevel = org_level->m_originalLevel;
        new_level->m_twoPlayerMode = org_level->m_twoPlayerMode;
        new_level->m_failedPasswordAttempts = org_level->m_failedPasswordAttempts;
        new_level->m_firstCoinVerified = org_level->m_firstCoinVerified;
        new_level->m_secondCoinVerified = org_level->m_secondCoinVerified;
        new_level->m_thirdCoinVerified = org_level->m_thirdCoinVerified;
        new_level->m_starsRequested = org_level->m_starsRequested;
        new_level->m_showedSongWarning = org_level->m_showedSongWarning;
        new_level->m_starRatings = org_level->m_starRatings;
        new_level->m_starRatingsSum = org_level->m_starRatingsSum;
        new_level->m_maxStarRatings = org_level->m_maxStarRatings;
        new_level->m_minStarRatings = org_level->m_minStarRatings;
        new_level->m_demonVotes = org_level->m_demonVotes;
        new_level->m_rateStars = org_level->m_rateStars;
        new_level->m_rateFeature = org_level->m_rateFeature;
        new_level->m_rateUser = org_level->m_rateUser;
        new_level->m_dontSave = org_level->m_dontSave;
        new_level->m_levelNotDownloaded = org_level->m_levelNotDownloaded;
        new_level->m_requiredCoins = org_level->m_requiredCoins;
        new_level->m_isUnlocked = org_level->m_isUnlocked;
        new_level->m_lastCameraPos = org_level->m_lastCameraPos;
        new_level->m_fastEditorZoom = org_level->m_fastEditorZoom;
        new_level->m_lastBuildTab = org_level->m_lastBuildTab;
        new_level->m_lastBuildPage = org_level->m_lastBuildPage;
        new_level->m_lastBuildGroupID = org_level->m_lastBuildGroupID;
        new_level->m_levelType = org_level->m_levelType;
        new_level->m_M_ID = org_level->m_M_ID;
        new_level->m_tempName = org_level->m_tempName;
        new_level->m_capacityString = org_level->m_capacityString;
        new_level->m_highObjectsEnabled = org_level->m_highObjectsEnabled;
        new_level->m_unlimitedObjectsEnabled = org_level->m_unlimitedObjectsEnabled;
        new_level->m_personalBests = org_level->m_personalBests;
        new_level->m_timestamp = org_level->m_timestamp;
        new_level->m_listPosition = org_level->m_listPosition;
        new_level->m_songIDs = org_level->m_songIDs;
        new_level->m_sfxIDs = org_level->m_sfxIDs;
        new_level->m_54 = org_level->m_54;
        new_level->m_bestTime = org_level->m_bestTime;
        new_level->m_bestPoints = org_level->m_bestPoints;
        new_level->m_k111 = org_level->m_k111;
        new_level->m_unkString3 = org_level->m_unkString3;
        new_level->m_unkString4 = org_level->m_unkString4;
        return new_level;
    }
    inline void logLevel(GJGameLevel* level) {
        log::debug( 
            {
            "{}:\n"
            "cocos2d::CCDictionary* m_lastBuildSave = {}\n"
            "geode::SeedValueRSV level->m_levelID = {}\n"
            "gd::string level->m_levelName = {}\n"
            "gd::string level->m_levelDesc = {}\n"
            "gd::string level->m_levelString = {}\n"
            "gd::string level->m_creatorName = {}\n"
            "gd::string level->m_recordString = {}\n"
            "gd::string level->m_uploadDate = {}\n"
            "gd::string level->m_updateDate = {}\n"
            "gd::string level->m_unkString1 = {}\n"
            "gd::string level->m_unkString2 = {}\n"
            "cocos2d::CCPoint level->m_unkPoint = {}\n"
            "geode::SeedValueRSV level->m_userID = {}\n"
            "geode::SeedValueRSV level->m_accountID = {}\n"
            "GJDifficulty level->m_difficulty = {}\n"
            "int level->m_audioTrack = {}\n"
            "int level->m_songID = {}\n"
            "int level->m_levelRev = {}\n"
            "bool level->m_unlisted = {}\n"
            "bool level->m_friendsOnly = {}\n"
            "geode::SeedValueRSV level->m_objectCount = {}\n"
            "int level->m_levelIndex = {}\n"
            "int level->m_ratings = {}\n"
            "int level->m_ratingsSum = {}\n"
            "int level->m_downloads = {}\n"
            "bool level->m_isEditable = {}\n"
            "bool level->m_gauntletLevel = {}\n"
            "bool level->m_gauntletLevel2 = {}\n"
            "int level->m_workingTime = {}\n"
            "int level->m_workingTime2 = {}\n"
            "bool level->m_lowDetailMode = {}\n"
            "bool level->m_lowDetailModeToggled = {}\n"
            "bool level->m_disableShakeToggled = {}\n"
            "bool level->m_selected = {}\n"
            "bool level->m_localOrSaved = {}\n"
            "bool level->m_disableShake = {}\n"
            "geode::SeedValueRS level->m_isVerified = {}\n"
            "bool level->m_isVerifiedRaw = {}\n"
            "bool level->m_isUploaded = {}\n"
            "bool level->m_hasBeenModified = {}\n"
            "int level->m_levelVersion = {}\n"
            "int level->m_gameVersion = {}\n"
            "geode::SeedValueRSV level->m_attempts = {}\n"
            "geode::SeedValueRSV level->m_jumps = {}\n"
            "geode::SeedValueRSV level->m_clicks = {}\n"
            "geode::SeedValueRSV level->m_attemptTime = {}\n"
            "int level->m_chk = {}\n"
            "bool level->m_isChkValid = {}\n"
            "bool level->m_isCompletionLegitimate = {}\n"
            "geode::SeedValueVSR level->m_normalPercent = {}\n"
            "geode::SeedValueRSV level->m_orbCompletion = {}\n"
            "geode::SeedValueRSV level->m_newNormalPercent2 = {}\n"
            "int level->m_practicePercent = {}\n"
            "int level->m_likes = {}\n"
            "int level->m_dislikes = {}\n"
            "int level->m_levelLength = {}\n"
            "int level->m_featured = {}\n"
            "int level->m_isEpic = {}\n"
            "bool level->m_levelFavorited = {}\n"
            "int level->m_levelFolder = {}\n"
            "geode::SeedValueRSV level->m_dailyID = {}\n"
            "geode::SeedValueRSV level->m_demon = {}\n"
            "int level->m_demonDifficulty = {}\n"
            "geode::SeedValueRSV level->m_stars = {}\n"
            "bool level->m_autoLevel = {}\n"
            "int level->m_coins = {}\n"
            "geode::SeedValueRSV level->m_coinsVerified = {}\n"
            "geode::SeedValueRS level->m_password = {}\n"
            "geode::SeedValueRSV level->m_originalLevel = {}\n"
            "bool level->m_twoPlayerMode = {}\n"
            "int level->m_failedPasswordAttempts = {}\n"
            "geode::SeedValueRSV level->m_firstCoinVerified = {}\n"
            "geode::SeedValueRSV level->m_secondCoinVerified = {}\n"
            "geode::SeedValueRSV level->m_thirdCoinVerified = {}\n"
            "int level->m_starsRequested = {}\n"
            "bool level->m_showedSongWarning = {}\n"
            "int level->m_starRatings = {}\n"
            "int level->m_starRatingsSum = {}\n"
            "int level->m_maxStarRatings = {}\n"
            "int level->m_minStarRatings = {}\n"
            "int level->m_demonVotes = {}\n"
            "int level->m_rateStars = {}\n"
            "int level->m_rateFeature = {}\n"
            "gd::string level->m_rateUser = {}\n"
            "bool level->m_dontSave = {}\n"
            "bool level->m_levelNotDownloaded = {}\n"
            "int level->m_requiredCoins = {}\n"
            "bool level->m_isUnlocked = {}\n"
            "cocos2d::CCPoint level->m_lastCameraPos = {}\n"
            "float level->m_fastEditorZoom = {}\n"
            "int level->m_lastBuildTab = {}\n"
            "int level->m_lastBuildPage = {}\n"
            "int level->m_lastBuildGroupID = {}\n"
            "GJLevelType level->m_levelType = {}\n"
            "int level->m_level->m_ID = {}\n"
            "gd::string level->m_tempName = {}\n"
            "gd::string level->m_capacityString = {}\n"
            "bool level->m_highObjectsEnabled = {}\n"
            "bool level->m_unlimitedObjectsEnabled = {}\n"
            "gd::string level->m_personalBests = {}\n"
            "int level->m_timestamp = {}\n"
            "int level->m_listPosition = {}\n"
            "gd::string level->m_songIDs = {}\n"
            "gd::string level->m_sfxIDs = {}\n"
            "int level->m_54 = {}\n"
            "int level->m_bestTime = {}\n"
            "int level->m_bestPoints = {}\n"
            "int level->m_k111 = {}\n"
            "gd::string level->m_unkString3 = {}\n"
            "gd::string level->m_unkString4 = {}\n"
            },
level,
level->m_lastBuildSave,//level->m_lastBuildSave;
level->m_levelID.value(),//level->m_levelID;
level->m_levelName,//level->m_levelName;
level->m_levelDesc,//level->m_levelDesc;
level->m_levelString,//level->m_levelString;
level->m_creatorName,//level->m_creatorName;
level->m_recordString,//level->m_recordString;
level->m_uploadDate,//level->m_uploadDate;
level->m_updateDate,//level->m_updateDate;
level->m_unkString1,//level->m_unkString1;
level->m_unkString2,//level->m_unkString2;
level->m_unkPoint,//level->m_unkPoint;
level->m_userID.value(),//level->m_userID;
level->m_accountID.value(),//level->m_accountID;
(int)level->m_difficulty,//level->m_difficulty;
level->m_audioTrack,//level->m_audioTrack;
level->m_songID,//level->m_songID;
level->m_levelRev,//level->m_levelRev;
level->m_unlisted,//level->m_unlisted;
level->m_friendsOnly,//level->m_friendsOnly;
level->m_objectCount.value(),//level->m_objectCount;
level->m_levelIndex,//level->m_levelIndex;
level->m_ratings,//level->m_ratings;
level->m_ratingsSum,//level->m_ratingsSum;
level->m_downloads,//level->m_downloads;
level->m_isEditable,//level->m_isEditable;
level->m_gauntletLevel,//level->m_gauntletLevel;
level->m_gauntletLevel2,//level->m_gauntletLevel2;
level->m_workingTime,//level->m_workingTime;
level->m_workingTime2,//level->m_workingTime2;
level->m_lowDetailMode,//level->m_lowDetailMode;
level->m_lowDetailModeToggled,//level->m_lowDetailModeToggled;
level->m_disableShakeToggled,//level->m_disableShakeToggled;
level->m_selected,//level->m_selected;
level->m_localOrSaved,//level->m_localOrSaved;
level->m_disableShake,//level->m_disableShake;
level->m_isVerified.value(),//level->m_isVerified;
level->m_isVerifiedRaw,//level->m_isVerifiedRaw;
level->m_isUploaded,//level->m_isUploaded;
level->m_hasBeenModified,//level->m_hasBeenModified;
level->m_levelVersion,//level->m_levelVersion;
level->m_gameVersion,//level->m_gameVersion;
level->m_attempts.value(),//level->m_attempts;
level->m_jumps.value(),//level->m_jumps;
level->m_clicks.value(),//level->m_clicks;
level->m_attemptTime.value(),//level->m_attemptTime;
level->m_chk,//level->m_chk;
level->m_isChkValid,//level->m_isChkValid;
level->m_isCompletionLegitimate,//level->m_isCompletionLegitimate;
level->m_normalPercent.value(),//level->m_normalPercent;
level->m_orbCompletion.value(),//level->m_orbCompletion;
level->m_newNormalPercent2.value(),//level->m_newNormalPercent2;
level->m_practicePercent,//level->m_practicePercent;
level->m_likes,//level->m_likes;
level->m_dislikes,//level->m_dislikes;
level->m_levelLength,//level->m_levelLength;
level->m_featured,//level->m_featured;
level->m_isEpic,//level->m_isEpic;
level->m_levelFavorited,//level->m_levelFavorited;
level->m_levelFolder,//level->m_levelFolder;
level->m_dailyID.value(),//level->m_dailyID;
level->m_demon.value(),//level->m_demon;
level->m_demonDifficulty,//level->m_demonDifficulty;
level->m_stars.value(),//level->m_stars;
level->m_autoLevel,//level->m_autoLevel;
level->m_coins,//level->m_coins;
level->m_coinsVerified.value(),//level->m_coinsVerified;
level->m_password.value(),//level->m_password;
level->m_originalLevel.value(),//level->m_originalLevel;
level->m_twoPlayerMode,//level->m_twoPlayerMode;
level->m_failedPasswordAttempts,//level->m_failedPasswordAttempts;
level->m_firstCoinVerified.value(),//level->m_firstCoinVerified;
level->m_secondCoinVerified.value(),//level->m_secondCoinVerified;
level->m_thirdCoinVerified.value(),//level->m_thirdCoinVerified;
level->m_starsRequested,//level->m_starsRequested;
level->m_showedSongWarning,//level->m_showedSongWarning;
level->m_starRatings,//level->m_starRatings;
level->m_starRatingsSum,//level->m_starRatingsSum;
level->m_maxStarRatings,//level->m_maxStarRatings;
level->m_minStarRatings,//level->m_minStarRatings;
level->m_demonVotes,//level->m_demonVotes;
level->m_rateStars,//level->m_rateStars;
level->m_rateFeature,//level->m_rateFeature;
level->m_rateUser,//level->m_rateUser;
level->m_dontSave,//level->m_dontSave;
level->m_levelNotDownloaded,//level->m_levelNotDownloaded;
level->m_requiredCoins,//level->m_requiredCoins;
level->m_isUnlocked,//level->m_isUnlocked;
level->m_lastCameraPos,//level->m_lastCameraPos;
level->m_fastEditorZoom,//level->m_fastEditorZoom;
level->m_lastBuildTab,//level->m_lastBuildTab;
level->m_lastBuildPage,//level->m_lastBuildPage;
level->m_lastBuildGroupID,//level->m_lastBuildGroupID;
(int)level->m_levelType,//level->m_levelType;
level->m_levelID.value(),//level->m_level->m_ID;
level->m_tempName,//level->m_tempName;
level->m_capacityString,//level->m_capacityString;
level->m_highObjectsEnabled,//level->m_highObjectsEnabled;
level->m_unlimitedObjectsEnabled,//level->m_unlimitedObjectsEnabled;
level->m_personalBests,//level->m_personalBests;
level->m_timestamp,//level->m_timestamp;
level->m_listPosition,//level->m_listPosition;
level->m_songIDs,//level->m_songIDs;
level->m_sfxIDs,//level->m_sfxIDs;
level->m_54,//level->m_54;
level->m_bestTime,//level->m_bestTime;
level->m_bestPoints,//level->m_bestPoints;
level->m_k111,//level->m_k111;
level->m_unkString3,//level->m_unkString3;
level->m_unkString4//level->m_unkString4;
                );
    }
};

namespace mle_audiotools {
    class Audio {
    public:
        std::string m_fileName;
        std::string m_title;
        std::string m_url;
        int m_artistID;
        int m_audioID;
        matjson::Value m_json = matjson::Value();
        inline static bool CallDefaults = false;
        static void updateFromFile(Audio* pAudio) {
            //update by existing
            auto song_meta_file = audios_meta_path / fmt::format("{}.json", pAudio->m_audioID);
            if (!fs::exists(song_meta_file)) return;
            auto file_content = fs::read(song_meta_file);
            //file parse
            auto value = matjson::parse("{}").unwrapOrDefault();
            auto error = std::string();
            auto parse = matjson::parse(file_content, error);
            if (parse.isOk()) value = parse.unwrapOrDefault();
            //set from json json
            pAudio->m_fileName = value["m_fileName"].asString().unwrapOr(pAudio->m_fileName.data()).data();
            pAudio->m_title = value["m_title"].asString().unwrapOr(pAudio->m_title.data()).data();
            pAudio->m_url = value["m_url"].asString().unwrapOr(pAudio->m_url.data()).data();
            pAudio->m_artistID = value["m_artistID"].asInt().unwrapOr(pAudio->m_artistID);
        }
        auto resetJson() {
            this->m_json.set("m_fileName", this->m_fileName);
            this->m_json.set("m_title", this->m_title);
            this->m_json.set("m_url", this->m_url);
            this->m_json.set("m_artistID", this->m_artistID);
            return this->m_json;
        }
        void openInfoLayer();
        Audio(int id = 1, bool noDefaults = true) {
            //default
            this->m_audioID = id;
            if (not noDefaults) {
                CallDefaults = 1;
                this->m_fileName = LevelTools::getAudioFileName(id).data();
                this->m_title = LevelTools::getAudioTitle(id).data();
                this->m_url = LevelTools::urlForAudio(id).data();
                this->m_artistID = LevelTools::artistForAudio(id);
                CallDefaults = 0;
            }
            //set vars from
            Audio::updateFromFile(this);
            //json
            resetJson();
            //save json
            if (not noDefaults and this->m_title == "Unknown") return;
            auto song_meta_file = audios_meta_path / fmt::format("{}.json", id);
            std::ofstream(song_meta_file) << this->m_json.dump(matjson::TAB_INDENTATION);
        };
    };
    class Artist {
    public:
        std::string m_ytURL;
        std::string m_ngURL;
        std::string m_name;
        int m_artistID;
        matjson::Value m_json;
        inline static bool CallDefaults = false;
        inline static void updateFromFile(Artist* pAudio) {
            //update by existing
            auto song_meta_file = artists_meta_path / fmt::format("{}.json", pAudio->m_artistID);
            if (!fs::exists(song_meta_file)) return;
            auto file_content = fs::read(song_meta_file);
            //file parse
            auto value = matjson::parse("{}").unwrapOrDefault();
            auto error = std::string();
            auto parse = matjson::parse(file_content, error);
            if (parse.isOk()) value = parse.unwrapOrDefault();
            //set from json json
            pAudio->m_ytURL = value["m_ytURL"].asString().unwrapOr(pAudio->m_ytURL.data()).data();
            pAudio->m_ngURL = value["m_ngURL"].asString().unwrapOr(pAudio->m_ngURL.data()).data();
            pAudio->m_name = value["m_name"].asString().unwrapOr(pAudio->m_name.data()).data();
            pAudio->m_artistID = value["m_artistID"].asInt().unwrapOr(pAudio->m_artistID);
        }
        auto resetJson() {
            this->m_json.set("m_ytURL", this->m_ytURL);
            this->m_json.set("m_ngURL", this->m_ngURL);
            this->m_json.set("m_name", this->m_name);
            return this->m_json;
        };
        Artist(int id = 1, bool noDefaults = true) {
            //default
            this->m_artistID = id;
            if (not noDefaults) {
                CallDefaults = 1;
                this->m_ytURL = LevelTools::ytURLForArtist(id).data();
                this->m_ngURL = LevelTools::ngURLForArtist(id).data();
                this->m_name = LevelTools::nameForArtist(id).data();
                CallDefaults = 0;
            }
            //set vars from
            Artist::updateFromFile(this);
            //json
            resetJson();
            //save json
            if (not noDefaults and this->m_name == " ") return;
            auto song_meta_file = artists_meta_path / fmt::format("{}.json", id);
            std::ofstream(song_meta_file) << this->m_json.dump(matjson::TAB_INDENTATION);
        };
    }; 
    inline void Audio::openInfoLayer() {
        auto artist = Artist(this->m_artistID);
        auto sobj = SongInfoObject::create(this->m_audioID, this->m_title, artist.m_name, artist.m_artistID, 1337.f, artist.m_ngURL, artist.m_ytURL, this->m_url, 0, "", 0, 0, 0);
        auto asd = CustomSongWidget::create(sobj, nullptr, 0, 0, 0, 1, 0, 0, 0);
        asd->onMore(asd);
    };
}

namespace mle_ui {
    inline auto deleteButtonSprite() {
        auto hi = geode::AccountButtonSprite::create(
            CCSprite::createWithSpriteFrameName("edit_delBtn_001.png"),
            AccountBaseColor::Gray
        );
        hi->setScale(0.8f);
        typeinfo_cast<CCSprite*>(hi->getChildren()->objectAtIndex(0))->setScale(1.1f);
        return hi;
    }
    inline auto settingsButtonSprite() {
        auto hi = geode::AccountButtonSprite::create(
            CCSprite::createWithSpriteFrameName("d_cogwheel_04_001.png"),
            AccountBaseColor::Gray
        );
        hi->setScale(0.8f);
        typeinfo_cast<CCSprite*>(hi->getChildren()->objectAtIndex(0))->setScale(1.1f);
        typeinfo_cast<CCSprite*>(hi->getChildren()->objectAtIndex(0))->setColor({ 20, 20, 20 });
        hi->addChild(typeinfo_cast<CCSprite*>(hi->getChildren()->objectAtIndex(0)));//make it darker
        return hi;
    }
    class ArtistsLayer : public GJDropDownLayer, CCScrollLayerExtDelegate {
    public:
        inline static CCPoint lastScrollPos = CCPoint(999.f, 0.f);
        ScrollLayer* m_scrollLayer = nullptr;
        struct FileNumbericSort {
            bool operator()(const fs::path& a, const fs::path& b) const {
                auto stra = std::regex_replace(a.filename().string(), std::regex(a.filename().extension().string()), "");
                auto strb = std::regex_replace(b.filename().string(), std::regex(b.filename().extension().string()), "");
                auto inta = utils::numFromString<int>(stra).unwrapOr(0);
                auto intb = utils::numFromString<int>(strb).unwrapOr(0);
                return inta <= intb;
            }
        };
        auto addArtistsCell(int id, bool& altBg, CCContentLayer* contentLayer) {
            auto width = contentLayer->getContentWidth();
            //hr
            if (altBg) contentLayer->addChild(CCLayerColor::create(
                ccColor4B(161 - 20, 88 - 20, 44 - 20, 255),
                width, 1.00f
            ));
            //item a 
            auto entry = CCLayerColor::create(
                altBg ? ccColor4B(161, 88, 44, 255) : ccColor4B(194, 114, 62, 255),
                width, 60.000f
            );
            entry->setLayout(AnchorLayout::create());
            //content
            auto artist = mle_audiotools::Artist(id);
            auto btnAspectWidth = width - 100.0f;
            auto nameStr = fmt::format("{}", artist.m_name);
            if (auto nameLabel = CCLabelBMFont::create(nameStr.c_str(), "bigFont.fnt")) {
                nameLabel->setScale(0.7f);
                nameLabel->setAnchorPoint(CCPoint(0.f, 0.5f));
                //fitUpScale
                if (nameLabel->getContentWidth() > btnAspectWidth) {
                    auto fitUpScale = btnAspectWidth / nameLabel->getContentWidth();
                    if (fitUpScale < nameLabel->getScale()) nameLabel->setScale(fitUpScale);
                }
                //point
                nameLabel->setLayoutOptions(
                    AnchorLayoutOptions::create()
                    ->setAnchor(Anchor::TopLeft)
                    ->setOffset(CCPoint(10.f, -20.f))
                );
                //add
                entry->addChild(nameLabel);
                entry->updateLayout();
            }
            auto idStr = fmt::format("ID: {}", artist.m_artistID);
            if (auto artistLabel = CCLabelBMFont::create(idStr.c_str(), "goldFont.fnt")) {
                artistLabel->setScale(0.7f);
                artistLabel->setAnchorPoint(CCPoint(0.f, 0.5f));
                //fitUpScale
                if (artistLabel->getContentWidth() > btnAspectWidth) {
                    auto fitUpScale = btnAspectWidth / artistLabel->getContentWidth();
                    if (fitUpScale < artistLabel->getScale()) artistLabel->setScale(fitUpScale);
                }
                //point
                artistLabel->setLayoutOptions(
                    AnchorLayoutOptions::create()
                    ->setAnchor(Anchor::BottomLeft)
                    ->setOffset(CCPoint(10.f, 19.f))
                );
                //add
                entry->addChild(artistLabel);
                entry->updateLayout();
            }
            if (auto viewBtnSprite = ButtonSprite::create("View", 1.f)) {
                viewBtnSprite->m_BGSprite->setContentSize(CCSize(62.65f, 30.f));
                viewBtnSprite->m_label->setFntFile("bigFont.fnt");
                viewBtnSprite->m_label->setScale(0.6f);
                auto menu = CCMenu::createWithItem(
                    CCMenuItemSpriteExtra::create(viewBtnSprite, this, menu_selector(ArtistsLayer::onView))
                );
                menu->getChildByType<CCMenuItemSpriteExtra>(0)->setTag(id);
                menu->setLayoutOptions(
                    AnchorLayoutOptions::create()
                    ->setAnchor(Anchor::Right)
                    ->setOffset(CCPoint(-42.f, 0.f))
                );
                entry->addChild(menu);
                entry->updateLayout();
            }
            //add
            contentLayer->addChild(entry, 0, id);
            contentLayer->setContentHeight(
                contentLayer->getContentHeight() + 1.1f + entry->getContentHeight()
            );
            //hr
            if (altBg) contentLayer->addChild(CCLayerColor::create(
                ccColor4B(161 - 20, 88 - 20, 44 - 20, 255),
                width, 1.00f
            ));
            //sw alt
            altBg = !altBg;
            return entry;
        }
        static auto create() {
            auto rtn = new ArtistsLayer;
            rtn->init("Artists Dir");
            //ScrollLayer::create
            rtn->m_scrollLayer = ScrollLayer::create(rtn->m_listLayer->getContentSize());
            rtn->m_scrollLayer->setID("scrollLayer"_spr);
            rtn->m_scrollLayer->m_delegate = rtn;
            rtn->m_listLayer->addChild(rtn->m_scrollLayer);
            //fill contentLayer
            auto& contentLayer = rtn->m_scrollLayer->m_contentLayer;
            contentLayer->setContentHeight(0.f);
            contentLayer->setLayout(
                ColumnLayout::create()
                ->setGap(0)
                ->setAxisReverse(true)
                ->setAxisAlignment(AxisAlignment::End)
            );
            auto altBg = true;
            std::set<fs::path, FileNumbericSort> aFileNumbericSortSet;
            for (auto& entry : fs::directory_iterator(artists_meta_path, fs::last_err_code))
                aFileNumbericSortSet.insert(entry.path());
            for (auto& filepath : aFileNumbericSortSet) {
                if (filepath.filename().extension() == ".json") {
                    auto name = std::regex_replace(
                        filepath.filename().string(),
                        std::regex(filepath.filename().extension().string()),
                        ""
                    );
                    auto id = utils::numFromString<int>(name).unwrapOr(0);
                    rtn->addArtistsCell(id, altBg, contentLayer);
                };
            }
            //add new btn
            if (auto newArtistBtnCell = rtn->addArtistsCell(0, altBg, contentLayer)) {
                newArtistBtnCell->removeAllChildren();
                auto item = CCMenuItemSpriteExtra::create(
                    ButtonSprite::create("Add Artist"),
                    rtn, menu_selector(ArtistsLayer::onNew)
                );
                auto menu = CCMenu::createWithItem(item);
                menu->setPosition(newArtistBtnCell->getContentSize() / 2);
                newArtistBtnCell->addChild(menu);
            }
            //update after adding
            contentLayer->setContentHeight(
                contentLayer->getContentHeight() > contentLayer->getParent()->getContentHeight() ?
                contentLayer->getContentHeight() :
                contentLayer->getParent()->getContentHeight()
            );
            contentLayer->updateLayout();
            rtn->m_scrollLayer->scrollToTop();
            if (lastScrollPos.x < 999.f) contentLayer->setPositionY(lastScrollPos.y);
            return rtn;
        }
        virtual void scrollViewDidEndMoving(CCScrollLayerExt* p0) {
            if (p0->m_contentLayer->getPositionY() < 0)
                lastScrollPos = p0->m_contentLayer->getPosition();
        }
        void onView(CCObject* obj);
        void onNew(CCObject* obj) {
            auto last_id = this->m_scrollLayer->m_contentLayer->getChildren()->count();
            for (last_id; last_id >= 0; last_id--) {
                if (this->m_scrollLayer->m_contentLayer->getChildByTag(last_id)) break;
            }
            auto artist = mle_audiotools::Artist(last_id + 1);
            this->hideLayer(1);
            auto newArtistsLayer = create();
            newArtistsLayer->setZOrder(this->getZOrder());
            newArtistsLayer->showLayer(1);
            newArtistsLayer->m_scrollLayer->m_contentLayer->setPositionY(0.f);
        }
    };
    class LevelsListMDPopup : public FLAlertLayer, FLAlertLayerProtocol, CCScrollLayerExtDelegate {
    public:
        inline static CCPoint lastScrollPos = CCPoint(999.f, 0.f);
        MDTextArea* m_md = nullptr;
        struct FileNumbericSort {
            bool operator()(const fs::path& a, const fs::path& b) const {
                auto stra = std::regex_replace(a.filename().string(), std::regex(a.filename().extension().string()), "");
                auto strb = std::regex_replace(b.filename().string(), std::regex(b.filename().extension().string()), "");
                auto inta = utils::numFromString<int>(stra).unwrapOr(0);
                auto intb = utils::numFromString<int>(strb).unwrapOr(0);
                return inta <= intb;
            }
        };
        static auto create() {
            auto rtn = new LevelsListMDPopup;
            rtn->FLAlertLayer::init(
                rtn,
                "Listing all levels in dir",
                "\n \n \n \n \n \n \n \n ",
                "Close", nullptr, 375.000f, 0, 0.000f, 1.f
            );
            rtn->setID("LevelsListMDPopup");
            rtn->customSetup();
            return rtn;
        }
        void customSetup() {
            auto md_size = this->getChildByIDRecursive("background")->getContentSize() - 110;
            auto md_str = std::string("# Dir Iter"); {
                std::set<fs::path, FileNumbericSort> aFileNumbericSortSet;
                for (auto& entry : fs::directory_iterator(levels_meta_path, fs::last_err_code))
                    aFileNumbericSortSet.insert(entry.path());
                for (auto& filepath : aFileNumbericSortSet) {
                    if (filepath.extension() == ".json") {
                        //json val
                        auto value = matjson::parse("{}").unwrapOrDefault();
                        auto error = std::string();
                        auto parse = matjson::parse(fs::read(filepath), error);
                        if (parse.isOk()) {
                            value = parse.unwrapOrDefault();
                            mle_leveltools::levelFromJson(value);
                            md_str.append(fmt::format(
                                "\n - {}: {}",
                                filepath.filename().string(),
                                value["main"]["m_levelName"].dump()
                            ));
                        }
                    };
                }
                for (auto& filepath : aFileNumbericSortSet) {
                    if (filepath.extension() == ".json") {
                        md_str.append(
                            "\n```\n" +
                            filepath.string().replace(
                                0, Mod::get()->getConfigDir().string().size() + 1, ""
                            ) +
                            ": " +
                            fs::read(filepath) +
                            "\n```\n\n---\n\n"
                        );
                    };
                }
            };
            if (m_md = MDTextArea::create(md_str, md_size)) {
                this->m_mainLayer->addChild(m_md, 1);
                m_md->setPosition(this->getContentSize() / 2);
                m_md->getScrollLayer()->m_delegate = this;
                if (lastScrollPos.x == 0.f)
                    m_md->getScrollLayer()->m_contentLayer->setPosition(lastScrollPos);
            }
            handleTouchPriority(this);
        }
        virtual void scrollViewDidEndMoving(CCScrollLayerExt* p0) {
            if (p0->m_contentLayer->getPositionY() < 0)
                lastScrollPos = p0->m_contentLayer->getPosition();
        }
    };
    class AudiosListMDPopup : public FLAlertLayer, FLAlertLayerProtocol, CCScrollLayerExtDelegate {
    public:
        inline static CCPoint lastScrollPos = CCPoint(999.f, 0.f);
        MDTextArea* m_md = nullptr;
        struct FileNumbericSort {
            bool operator()(const fs::path& a, const fs::path& b) const {
                auto stra = std::regex_replace(a.filename().string(), std::regex(a.filename().extension().string()), "");
                auto strb = std::regex_replace(b.filename().string(), std::regex(b.filename().extension().string()), "");
                auto inta = utils::numFromString<int>(stra).unwrapOr(0);
                auto intb = utils::numFromString<int>(strb).unwrapOr(0);
                return inta <= intb;
            }
        };
        static auto create() {
            auto rtn = new AudiosListMDPopup;
            rtn->FLAlertLayer::init(
                rtn,
                "Listing all audios in dir",
                "\n \n \n \n \n \n \n \n ",
                "Close", nullptr, 375.000f, 0, 0.000f, 1.f
            );
            rtn->setID("AudiosListMDPopup");
            rtn->customSetup();
            return rtn;
        }
        void customSetup() {
            auto md_size = this->getChildByIDRecursive("background")->getContentSize() - 110;
            auto md_str = std::string("# Dir Iter"); {
                std::set<fs::path, FileNumbericSort> aFileNumbericSortSet;
                for (auto& entry : fs::directory_iterator(audios_meta_path, fs::last_err_code))
                    aFileNumbericSortSet.insert(entry.path());
                for (auto& filepath : aFileNumbericSortSet) {
                    if (filepath.extension() == ".json") {
                        //json val
                        auto value = matjson::parse("{}").unwrapOrDefault();
                        auto error = std::string();
                        auto parse = matjson::parse(fs::read(filepath), error);
                        if (parse.isOk()) {
                            value = parse.unwrapOrDefault();
                            mle_leveltools::levelFromJson(value);
                            md_str.append(fmt::format(
                                "\n - {}: {}",
                                filepath.filename().string(),
                                value["m_title"].dump()
                            ));
                        }
                    };
                }
                for (auto& filepath : aFileNumbericSortSet) {
                    if (filepath.extension() == ".json") {
                        md_str.append(
                            "\n```\n" +
                            filepath.string().replace(
                                0, Mod::get()->getConfigDir().string().size() + 1, ""
                            ) +
                            ": " +
                            fs::read(filepath) +
                            "\n```\n\n---\n\n"
                        );
                    };
                }
            };
            if (m_md = MDTextArea::create(md_str, md_size)) {
                this->m_mainLayer->addChild(m_md, 1);
                m_md->setPosition(this->getContentSize() / 2);
                m_md->getScrollLayer()->m_delegate = this;
                if (lastScrollPos.x == 0.f)
                    m_md->getScrollLayer()->m_contentLayer->setPosition(lastScrollPos);
            }
            handleTouchPriority(this);
        }
        virtual void scrollViewDidEndMoving(CCScrollLayerExt* p0) {
            if (p0->m_contentLayer->getPositionY() < 0)
                lastScrollPos = p0->m_contentLayer->getPosition();
        }
    };
    class LevelConfigPopup : public FLAlertLayer, FLAlertLayerProtocol, TextInputDelegate {
    public:
        GJGameLevel* m_level = nullptr;
        std::function<void()> m_onSave = []() {};
        static auto create(GJGameLevel* for_level = LevelTools::getLevel(1, 0)) {
            auto rtn = new LevelConfigPopup;
            rtn->init(
                rtn,
                "Level Meta Config",
                "\n \n \n \n \n \n \n \n ",
                "Close", "Save", 352.000f, 0, 290.000f, 1.f
            );
            rtn->setID("LevelConfigPopup");
            rtn->m_level = for_level;
            rtn->customSetup();
            return rtn;
        }
        void onPlayMusic(CCObject* toggleItemObj) {
            auto m_audioTrack = typeinfo_cast<CCTextInputNode*>(this->getChildByIDRecursive("m_audioTrack")); if (not m_audioTrack) return;
            int audioTrack = utils::numFromString<int>(m_audioTrack->getString().data()).unwrapOr(m_level->m_audioTrack);
            auto toggleItem = typeinfo_cast<CCMenuItemToggler*>(toggleItemObj); if (not toggleItem) return;
            toggleItem->isOn() ?
                GameManager::get()->fadeInMenuMusic() :
                GameManager::get()->fadeInMusic(mle_audiotools::Audio(audioTrack).m_fileName);
        }
        void onBrowseSongs(CCObject* asd) {
            AudiosListMDPopup::create()->show();
        }
        void onLevelEdit(CCObject*) {
            m_level->m_levelType = GJLevelType::Editor;
            auto layer = LevelEditorLayer::create(
                mle_leveltools::replaceSecretCoinsByUserOnesInLevel(m_level), 0
            );
            auto mark = CCNode::create();
            mark->setID("IsMainLevelEditor");
            layer->addChild(mark);
            cocos::switchToScene(layer);
        }
        void customSetup() {
            //form_box
            auto form_box = CCLayer::create();
            if (form_box) {
                //base sets
                form_box->setID("form_box");
                form_box->setPosition(CCPoint(0.f, 116.f));
                form_box->setContentSize(CCSize(255.f, 175.f));
                form_box->ignoreAnchorPointForPosition(false);
                //add
                this->m_buttonMenu->addChild(form_box);
                //layout
                auto layout = ColumnLayout::create();
                layout->setAxisReverse(1);
                layout->setCrossAxisOverflow(0);
                form_box->setLayout(layout);
            }
            //addInputs
            auto addInput = [this, form_box](auto label = "", auto id = "", CommonFilter commonFilter = CommonFilter::Any) {
                auto input_box = CCLayer::create();
                input_box->setContentSize(CCSize(255.f, 45.000f));
                //layout
                auto layout = ColumnLayout::create();
                layout->setAxisReverse(1);
                layout->setCrossAxisOverflow(0);
                input_box->setLayout(layout);
                //labelNode
                auto labelNode = CCLabelBMFont::create(label, "bigFont.fnt");
                labelNode->setID(id + std::string("/label"));
                input_box->addChild(labelNode);
                //inputNode
                auto inputNode = TextInput::create(form_box->getContentWidth(), label, "chatFont.fnt");
                inputNode->getInputNode()->setID(id);
                inputNode->getInputNode()->setMaxLabelScale(1.f);
                inputNode->setCommonFilter(commonFilter);
                inputNode->setDelegate(this);
                input_box->addChild(inputNode);
                //upd
                input_box->updateLayout();
                //add
                input_box->updateLayout();
                form_box->addChild(input_box);
                form_box->updateLayout();
            };
            addInput("Name", "m_levelName");
            addInput("Audio Track", "m_audioTrack", CommonFilter::Int);
            addInput("Difficulty", "m_difficulty", CommonFilter::Int);
            addInput("Stars", "m_stars", CommonFilter::Int);
            addInput("Required Coins", "m_requiredCoins", CommonFilter::Int);
            loadInputs();
            //playMusic
            if (auto GJ_playMusicBtn_001 = CCSprite::createWithSpriteFrameName("GJ_playMusicBtn_001.png")) {
                auto GJ_stopMusicBtn_001 = CCSprite::createWithSpriteFrameName("GJ_stopMusicBtn_001.png");
                auto toggleItem = CCMenuItemToggler::create(
                    GJ_playMusicBtn_001, GJ_stopMusicBtn_001,
                    this, menu_selector(LevelConfigPopup::onPlayMusic)
                );
                toggleItem->setPosition(CCPoint(138.f, 212.f));
                this->m_buttonMenu->addChild(toggleItem);
            }
            //editLevel
            if (auto GJ_viewLevelsBtn_001 = CCSprite::createWithSpriteFrameName("GJ_viewLevelsBtn_001.png")) {
                auto editLevelItem = CCMenuItemSpriteExtra::create(
                    GJ_viewLevelsBtn_001,
                    this, menu_selector(LevelConfigPopup::onLevelEdit)
                );
                editLevelItem->setPosition(CCPoint(138.f, 168.000f));
                editLevelItem->m_baseScale = 0.825;
                editLevelItem->unselected();
                this->m_buttonMenu->addChild(editLevelItem);
            }
        }
        void loadInputs() {
            if (not this->m_level) return;
            auto m_levelName = typeinfo_cast<CCTextInputNode*>(this->getChildByIDRecursive("m_levelName"));
            auto m_audioTrack = typeinfo_cast<CCTextInputNode*>(this->getChildByIDRecursive("m_audioTrack"));
            auto m_difficulty = typeinfo_cast<CCTextInputNode*>(this->getChildByIDRecursive("m_difficulty"));
            auto m_stars = typeinfo_cast<CCTextInputNode*>(this->getChildByIDRecursive("m_stars"));
            auto m_requiredCoins = typeinfo_cast<CCTextInputNode*>(this->getChildByIDRecursive("m_requiredCoins"));
            if (m_levelName) m_levelName->setString(fmt::format("{}", this->m_level->m_levelName.data()));
            if (m_audioTrack) m_audioTrack->setString(fmt::format("{}", this->m_level->m_audioTrack));
            if (m_difficulty) m_difficulty->setString(fmt::format("{}", (int)this->m_level->m_difficulty));
            if (m_stars) m_stars->setString(fmt::format("{}", this->m_level->m_stars.value()));
            if (m_requiredCoins) m_requiredCoins->setString(fmt::format("{}", this->m_level->m_requiredCoins));
        }
        void applyInputs() {
            if (not this->m_level) return;
            auto m_levelName = typeinfo_cast<CCTextInputNode*>(this->getChildByIDRecursive("m_levelName"));
            auto m_audioTrack = typeinfo_cast<CCTextInputNode*>(this->getChildByIDRecursive("m_audioTrack"));
            auto m_difficulty = typeinfo_cast<CCTextInputNode*>(this->getChildByIDRecursive("m_difficulty"));
            auto m_stars = typeinfo_cast<CCTextInputNode*>(this->getChildByIDRecursive("m_stars"));
            auto m_requiredCoins = typeinfo_cast<CCTextInputNode*>(this->getChildByIDRecursive("m_requiredCoins"));
            if (m_levelName) m_level->m_levelName = m_levelName->getString();
            if (m_audioTrack) m_level->m_audioTrack = utils::numFromString<int>(m_audioTrack->getString().data()).unwrapOr(m_level->m_audioTrack);
            if (m_difficulty) m_level->m_difficulty = (GJDifficulty)utils::numFromString<int>(m_difficulty->getString().data()).unwrapOr((int)m_level->m_difficulty);
            if (m_stars) m_level->m_stars = utils::numFromString<int>(m_stars->getString().data()).unwrapOr(m_level->m_stars.value());
            if (m_requiredCoins) m_level->m_requiredCoins = utils::numFromString<int>(m_requiredCoins->getString().data()).unwrapOr(m_level->m_requiredCoins);
        }
        void textChanged(CCTextInputNode* p0) {
            //log::debug("{}({}) id: {}", __FUNCTION__, p0, p0->getID());
            if (p0->getID() == "m_difficulty") {
                auto difficulty = utils::numFromString<int>(p0->getString().data()).unwrapOr(0);
                GJDifficultySprite* difficultyPrev;
                if (difficultyPrev = typeinfo_cast<GJDifficultySprite*>(this->getChildByIDRecursive("difficultyPrev")));
                else {
                    difficultyPrev = GJDifficultySprite::create(difficulty, GJDifficultyName::Short);
                    difficultyPrev->setAnchorPoint(CCPoint(0.f, 0.f));
                    difficultyPrev->setPosition(CCPoint(260.f, 0.f));
                    difficultyPrev->setScale(1.2f);
                    difficultyPrev->setID("difficultyPrev");
                    p0->getParent()->addChild(difficultyPrev);
                }
                difficultyPrev->updateDifficultyFrame(difficulty, GJDifficultyName::Short);
            }
            if (p0->getID() == "m_audioTrack") {
                CCMenu* browseBtnMenu;
                if (browseBtnMenu = typeinfo_cast<CCMenu*>(this->getChildByIDRecursive("browseBtnMenu")));
                else {
                    //browseBtnSpr
                    auto browseBtnSpr = ButtonSprite::create("Browse", 0.5f);
                    auto browseBtn = CCMenuItemSpriteExtra::create(
                        browseBtnSpr, this, menu_selector(LevelConfigPopup::onBrowseSongs)
                    );
                    browseBtnMenu = CCMenu::createWithItem(browseBtn);
                    browseBtnMenu->setAnchorPoint(CCPoint(0.f, 0.f));
                    browseBtnMenu->setPosition(CCPoint(242.f, 6.f));
                    browseBtnMenu->setScale(0.825f);
                    browseBtnMenu->setID("browseBtnMenu");
                    p0->getParent()->addChild(browseBtnMenu);
                    handleTouchPriority(this);
                }
            }
        }
        void FLAlert_Clicked(FLAlertLayer* p0, bool p1) {
            GameManager::get()->fadeInMenuMusic();
            if (not p1) return;
            applyInputs();
            //json
            auto value = mle_leveltools::jsonFromLevel(this->m_level);
            //save json
            auto level_meta_file = levels_meta_path / fmt::format("{}.json", m_level->m_levelID.value());
            std::ofstream(level_meta_file) << value.dump(matjson::TAB_INDENTATION);
            //call custom func
            this->m_onSave();
        };
    };
    class AudioConfigPopup : public FLAlertLayer, FLAlertLayerProtocol, TextInputDelegate {
    public:
        std::function<void()> m_onSave = []() {};
        mle_audiotools::Audio m_audio = mle_audiotools::Audio(0);
        static auto create(int id = 0) {
            auto rtn = new AudioConfigPopup;
            rtn->init(
                rtn,
                "Audio Meta Config",
                "\n \n \n \n \n \n \n \n ",
                "Close", "Save", 352.000f, 0, 290.000f, 1.f
            );
            rtn->setID("AudioConfigPopup");
            rtn->m_audio = mle_audiotools::Audio(id);
            rtn->customSetup();
            return rtn;
        }
        void onPlayMusic(CCObject* toggleItemObj) {
            auto m_fileName = typeinfo_cast<CCTextInputNode*>(this->getChildByIDRecursive("m_fileName")); if (not m_fileName) return;
            auto toggleItem = typeinfo_cast<CCMenuItemToggler*>(toggleItemObj); if (not toggleItem) return;
            toggleItem->isOn() ?
                GameManager::get()->fadeInMenuMusic() :
                GameManager::get()->fadeInMusic(m_fileName->getString());
        }
        void onBrowseArtists(CCObject* asd) {
            auto pArtistsLayer = ArtistsLayer::create();
            pArtistsLayer->showLayer(0);
            pArtistsLayer->setZOrder(this->getZOrder());
        }
        void customSetup() {
            //form_box
            auto form_box = CCLayer::create();
            if (form_box) {
                //base sets
                form_box->setID("form_box");
                form_box->setPosition(CCPoint(0.f, 116.f));
                form_box->setContentSize(CCSize(255.f, 175.f));
                form_box->ignoreAnchorPointForPosition(false);
                //add
                this->m_buttonMenu->addChild(form_box);
                //layout
                auto layout = ColumnLayout::create();
                layout->setAxisReverse(1);
                layout->setCrossAxisOverflow(0);
                form_box->setLayout(layout);
            }
            //addInputs
            auto addInput = [this, form_box](auto label = "", auto id = "", CommonFilter commonFilter = CommonFilter::Any) {
                //labelNode
                auto labelNode = CCLabelBMFont::create(label, "bigFont.fnt");
                labelNode->setID(id + std::string("/label"));
                form_box->addChild(labelNode);
                //inputNode
                auto inputNode = TextInput::create(form_box->getContentWidth(), label, "chatFont.fnt");
                inputNode->getInputNode()->setID(id);
                inputNode->getInputNode()->setMaxLabelScale(1.f);
                inputNode->setCommonFilter(commonFilter);
                inputNode->setDelegate(this);
                form_box->addChild(inputNode);
                //upd
                form_box->updateLayout();
                }; 
            addInput("File Name", "m_fileName");
            addInput("Title", "m_title");
            addInput("URL", "m_url");
            addInput("Artist ID", "m_artistID", CommonFilter::Int);
            loadInputs();
            //playMusic
            if (auto GJ_playMusicBtn_001 = CCSprite::createWithSpriteFrameName("GJ_playMusicBtn_001.png")) {
                auto GJ_stopMusicBtn_001 = CCSprite::createWithSpriteFrameName("GJ_stopMusicBtn_001.png");
                auto toggleItem = CCMenuItemToggler::create(
                    GJ_playMusicBtn_001, GJ_stopMusicBtn_001,
                    this, menu_selector(AudioConfigPopup::onPlayMusic)
                );
                toggleItem->setPosition(CCPoint(138.f, 212.f));
                this->m_buttonMenu->addChild(toggleItem);
            }
        }
        void loadInputs() {
            auto m_fileName = typeinfo_cast<CCTextInputNode*>(this->getChildByIDRecursive("m_fileName"));
            auto m_title = typeinfo_cast<CCTextInputNode*>(this->getChildByIDRecursive("m_title"));
            auto m_url = typeinfo_cast<CCTextInputNode*>(this->getChildByIDRecursive("m_url"));
            auto m_artistID = typeinfo_cast<CCTextInputNode*>(this->getChildByIDRecursive("m_artistID"));
            if (m_fileName) m_fileName->setString(fmt::format("{}", this->m_audio.m_fileName.data()));
            if (m_title) m_title->setString(fmt::format("{}", this->m_audio.m_title.data()));
            if (m_url) m_url->setString(fmt::format("{}", this->m_audio.m_url.data()));
            if (m_artistID) m_artistID->setString(fmt::format("{}", this->m_audio.m_artistID));
        }
        void applyInputs() {
            auto m_fileName = typeinfo_cast<CCTextInputNode*>(this->getChildByIDRecursive("m_fileName"));
            auto m_title = typeinfo_cast<CCTextInputNode*>(this->getChildByIDRecursive("m_title"));
            auto m_url = typeinfo_cast<CCTextInputNode*>(this->getChildByIDRecursive("m_url"));
            auto m_artistID = typeinfo_cast<CCTextInputNode*>(this->getChildByIDRecursive("m_artistID"));
            if (m_fileName) this->m_audio.m_fileName = m_fileName->getString().data();
            if (m_title) this->m_audio.m_title = m_title->getString().data();
            if (m_url) this->m_audio.m_url = m_url->getString().data();
            if (m_artistID) this->m_audio.m_artistID = utils::numFromString<int>(m_artistID->getString().data()).unwrapOr(this->m_audio.m_artistID);
        }
        void textChanged(CCTextInputNode* p0) {
            //log::debug("{}({}) id: {}", __FUNCTION__, p0, p0->getID());
            if (p0->getID() == "m_fileName") {
                CCLabelBMFont* existanceIndicator;
                if (existanceIndicator = typeinfo_cast<CCLabelBMFont*>(this->getChildByIDRecursive("existanceIndicator")));
                else {
                    existanceIndicator = CCLabelBMFont::create("", "bigFont.fnt");
                    existanceIndicator->setAnchorPoint(CCPoint(0.f, -0.200f));
                    existanceIndicator->setPosition(CCPoint(260.f, 0.f));
                    existanceIndicator->setScale(0.3f);
                    existanceIndicator->setID("existanceIndicator");
                    p0->getParent()->addChild(existanceIndicator);
                }
                auto exist = fs::exists(p0->getString().data());
                existanceIndicator->setString(exist ? "file\nexists." : "file don't\nexists!");
                existanceIndicator->setColor(exist ? ccColor3B(85, 255, 100) : ccColor3B(255, 85, 85));
            }
            if (p0->getID() == "m_artistID") {
                CCMenu* browseBtnMenu;
                if (browseBtnMenu = typeinfo_cast<CCMenu*>(this->getChildByIDRecursive("browseBtnMenu")));
                else {
                    //browseBtnSpr
                    auto browseBtnSpr = ButtonSprite::create("Browse", 0.5f);
                    auto browseBtn = CCMenuItemSpriteExtra::create(
                        browseBtnSpr, this, menu_selector(AudioConfigPopup::onBrowseArtists)
                    );
                    browseBtnMenu = CCMenu::createWithItem(browseBtn);
                    browseBtnMenu->setAnchorPoint(CCPoint(0.f, 0.f));
                    browseBtnMenu->setPosition(CCPoint(242.f, 6.f));
                    browseBtnMenu->setScale(0.825f);
                    browseBtnMenu->setID("browseBtnMenu");
                    p0->getParent()->addChild(browseBtnMenu);
                    handleTouchPriority(this);
                }
            }
        }
        void FLAlert_Clicked(FLAlertLayer* p0, bool p1) {
            GameManager::get()->fadeInMenuMusic();
            if (not p1) return;
            applyInputs();
            //json
            auto value = m_audio.resetJson();
            //save json
            auto song_meta_file = audios_meta_path / fmt::format("{}.json", m_audio.m_audioID);
            std::ofstream(song_meta_file) << value.dump(matjson::TAB_INDENTATION);
            //call custom func
            this->m_onSave();
        };
    };
    class ArtistsConfigPopup : public FLAlertLayer, FLAlertLayerProtocol, TextInputDelegate {
    public:
        std::function<void()> m_onSave = []() {};
        mle_audiotools::Artist m_artist = mle_audiotools::Artist(0);
        static auto create(int id = 0) {
            auto rtn = new ArtistsConfigPopup;
            rtn->init(
                rtn,
                "Audio Meta Config",
                "\n \n \n \n \n \n \n \n ",
                "Close", "Save", 352.000f, 0, 290.000f, 1.f
            );
            rtn->setID("ArtistsConfigPopup");
            rtn->m_artist = mle_audiotools::Artist(id);
            rtn->customSetup();
            return rtn;
        }
        void onDelete(CCObject* asd) {
            //remove json
            auto song_meta_file = artists_meta_path / fmt::format("{}.json", m_artist.m_artistID);
            fs::remove(song_meta_file, fs::last_err_code);
            //reset layer mayb ya
            this->onBtn1(this);
            this->m_onSave();
        }
        void customSetup() {
            //form_box
            auto form_box = CCLayer::create();
            if (form_box) {
                //base sets
                form_box->setID("form_box");
                form_box->setPosition(CCPoint(0.f, 116.f));
                form_box->setContentSize(CCSize(255.f, 175.f));
                form_box->ignoreAnchorPointForPosition(false);
                //add
                this->m_buttonMenu->addChild(form_box);
                //layout
                auto layout = ColumnLayout::create();
                layout->setAxisReverse(1);
                layout->setCrossAxisOverflow(0);
                form_box->setLayout(layout);
            }
            //addInputs
            auto addInput = [this, form_box](auto label = "", auto id = "", CommonFilter commonFilter = CommonFilter::Any) {
                //labelNode
                auto labelNode = CCLabelBMFont::create(label, "bigFont.fnt");
                labelNode->setID(id + std::string("/label"));
                form_box->addChild(labelNode);
                //inputNode
                auto inputNode = TextInput::create(form_box->getContentWidth(), label, "chatFont.fnt");
                inputNode->getInputNode()->setID(id);
                inputNode->getInputNode()->setMaxLabelScale(1.f);
                inputNode->setCommonFilter(commonFilter);
                inputNode->setDelegate(this);
                form_box->addChild(inputNode);
                //upd
                form_box->updateLayout();
                }; 
            addInput("YouTube URL", "m_ytURL");
            addInput("NG URL", "m_ngURL");
            addInput("Name", "m_name");
            loadInputs();
            //delete
            if (auto deleteBtnSprite = deleteButtonSprite()) {
                auto toggleItem = CCMenuItemSpriteExtra::create(
                    deleteBtnSprite,
                    this, menu_selector(ArtistsConfigPopup::onDelete)
                );
                toggleItem->setPosition(CCPoint(138.f, 212.f));
                this->m_buttonMenu->addChild(toggleItem);
            }
        }
        void loadInputs() {
            auto m_ytURL = typeinfo_cast<CCTextInputNode*>(this->getChildByIDRecursive("m_ytURL"));
            auto m_ngURL = typeinfo_cast<CCTextInputNode*>(this->getChildByIDRecursive("m_ngURL"));
            auto m_name = typeinfo_cast<CCTextInputNode*>(this->getChildByIDRecursive("m_name"));
            if (m_ytURL) m_ytURL->setString(fmt::format("{}", this->m_artist.m_ytURL.data()));
            if (m_ngURL) m_ngURL->setString(fmt::format("{}", this->m_artist.m_ngURL.data()));
            if (m_name) m_name->setString(fmt::format("{}", this->m_artist.m_name.data()));
        }
        void applyInputs() {
            auto m_ytURL = typeinfo_cast<CCTextInputNode*>(this->getChildByIDRecursive("m_ytURL"));
            auto m_ngURL = typeinfo_cast<CCTextInputNode*>(this->getChildByIDRecursive("m_ngURL"));
            auto m_name = typeinfo_cast<CCTextInputNode*>(this->getChildByIDRecursive("m_name"));
            if (m_ytURL) this->m_artist.m_ytURL = m_ytURL->getString().data();
            if (m_ngURL) this->m_artist.m_ngURL = m_ngURL->getString().data();
            if (m_name) this->m_artist.m_name = m_name->getString().data();
        }
        void FLAlert_Clicked(FLAlertLayer* p0, bool p1) {
            GameManager::get()->fadeInMenuMusic();
            if (not p1) return;
            applyInputs();
            //save artist
            auto value = m_artist.resetJson();
            auto artist_meta_file = artists_meta_path / fmt::format("{}.json", m_artist.m_artistID);
            std::ofstream(artist_meta_file) << value.dump(matjson::TAB_INDENTATION);
            //call custom func
            this->m_onSave();
        };
    };
    inline void ArtistsLayer::onView(CCObject* obj) {
        auto pArtistsConfigPopup = ArtistsConfigPopup::create(obj->getTag());
        pArtistsConfigPopup->m_onSave = [this]() {
            this->hideLayer(1);
            auto newArtistsLayer = ArtistsLayer::create();
            newArtistsLayer->setZOrder(this->getZOrder());
            newArtistsLayer->showLayer(1);
            };
        pArtistsConfigPopup->show();
    };
    class CopyLevelPopup : public FLAlertLayer, FLAlertLayerProtocol, TextInputDelegate {
    public:
        std::function<void()> m_onSave = []() {};
        static auto create(GJGameLevel* for_level) {
            auto rtn = new CopyLevelPopup;
            rtn->init(
                rtn,
                "Grab Level To \"Main Levels\"...",
                "\n \n \n \n \n \n \n \n ",
                "Close", "Save", 375.000f, 0, 0.000f, 1.f
            );
            rtn->setID("CopyLevelPopup");
            auto level = mle_leveltools::levelFromLevel(for_level);
            //mle_leveltools::logLevel(level);
            level->setID("level");
            rtn->addChild(level);
            rtn->customSetup();
            return rtn;
        }
        void customSetup() {
            //form_box
            auto form_box = CCLayer::create();
            if (form_box) {
                //base sets
                form_box->setID("form_box");
                form_box->setPosition(CCPoint(0.f, 116.f));
                form_box->setContentSize(CCSize(255.f, 175.f));
                form_box->ignoreAnchorPointForPosition(false);
                //add
                this->m_buttonMenu->addChild(form_box);
                //layout
                auto layout = ColumnLayout::create();
                layout->setAxisReverse(1);
                form_box->setLayout(layout);
            }
            //addInputs
            auto addInput = [this, form_box](auto label = "", auto id = "", CommonFilter commonFilter = CommonFilter::Any) {
                //labelNode
                auto labelNode = CCLabelBMFont::create(label, "bigFont.fnt");
                labelNode->setID(id + std::string("/label"));
                form_box->addChild(labelNode);
                //inputNode
                auto inputNode = TextInput::create(form_box->getContentWidth(), label, "chatFont.fnt");
                inputNode->getInputNode()->setID(id);
                inputNode->getInputNode()->setMaxLabelScale(1.f);
                inputNode->setCommonFilter(commonFilter);
                inputNode->setDelegate(this);
                form_box->addChild(inputNode);
                //upd
                form_box->updateLayout();
                };
            addInput("Target ID", "m_levelID");
            loadInputs();
            //help
            form_box->addChild(CCLabelBMFont::create("Help", "bigFont.fnt"));
            form_box->updateLayout();
            if (auto menu = CCMenu::create()) {
                //layout
                auto layout = RowLayout::create();
                menu->setLayout(layout);
                //Levels List
                menu->addChild(CCMenuItemExt::createSpriteExtra(
                    ButtonSprite::create("Levels List", 0.6f),
                    [](auto) {
                        LevelsListMDPopup::create()->show();
                    }
                ));
                //Settings
                menu->addChild(CCMenuItemExt::createSpriteExtra(
                    ButtonSprite::create("Settings", 0.6f),
                    [](auto) {
                        openSettingsPopup(Mod::get());
                    }
                ));
                //lyupd
                menu->updateLayout();
                //add
                form_box->addChild(menu);
                form_box->updateLayout();
                handleTouchPriority(this);
            }
        }
        void loadInputs() {
            auto level = typeinfo_cast<GJGameLevel*>(this->getChildByIDRecursive("level"));
            if (not level) return;
            auto m_levelID = typeinfo_cast<CCTextInputNode*>(this->getChildByIDRecursive("m_levelID"));
            if (m_levelID) m_levelID->setString(fmt::format("{}", level->m_levelID.value()));
        }
        void applyInputs() {
            auto level = typeinfo_cast<GJGameLevel*>(this->getChildByIDRecursive("level"));
            if (not level) return;
            auto m_levelID = typeinfo_cast<CCTextInputNode*>(this->getChildByIDRecursive("m_levelID"));
            if (m_levelID) level->m_levelID = utils::numFromString<int>(m_levelID->getString().data()).unwrapOr(level->m_levelID.value());
        }
        void FLAlert_Clicked(FLAlertLayer* p0, bool p1) {
            auto __this = typeinfo_cast<CopyLevelPopup*>(p0);
            if (not __this) return;
            auto level = typeinfo_cast<GJGameLevel*>(__this->getChildByIDRecursive("level"));
            if (not level) return; 
            __this->applyInputs();
            //update level
            level->m_levelType = GJLevelType::Local;
            level->m_difficulty = (GJDifficulty)level->getAverageDifficulty();
            if (level->m_demon.value()) level->m_difficulty = (GJDifficulty)(4 + level->m_demonDifficulty);
            //save json
            mle_leveltools::updateLevelDataAndMetaFiles(level->m_levelString, level);
            //call custom func
            __this->m_onSave();
        };
    };
    class CopyAudioPopup : public FLAlertLayer, FLAlertLayerProtocol, TextInputDelegate {
    public:
        std::function<void()> m_onSave = []() {};
        mle_audiotools::Audio m_audio = mle_audiotools::Audio(0);
        mle_audiotools::Artist m_artist = mle_audiotools::Artist(0);
        Ref<CCNode> refSongInfoLayer;
        static auto create(CCNode* pSongInfoLayer) {
            auto rtn = new CopyAudioPopup;
            rtn->init(
                rtn,
                "Convert To Audio and Artist...",
                "\n \n \n \n \n \n \n \n ",
                "Close", "Save", 395.000f, 0, 0.000f, 1.f
            );
            rtn->setID("CopyAudioPopup");
            rtn->refSongInfoLayer = pSongInfoLayer;
            rtn->m_artist = mle_audiotools::Artist(findDataNode(pSongInfoLayer, "m_artistID")->getTag());
            rtn->m_artist.m_name = findDataNode(pSongInfoLayer, "m_artistName")->getString();
            rtn->m_artist.m_ngURL = fmt::format("https://{}.newgrounds.com/audio", rtn->m_artist.m_name);
            rtn->m_audio = mle_audiotools::Audio(findDataNode(pSongInfoLayer, "m_songID")->getTag());
            rtn->m_audio.m_title = findDataNode(pSongInfoLayer, "m_songName")->getString();
            rtn->m_audio.m_url = fmt::format("https://www.newgrounds.com/audio/listen/{}", rtn->m_audio.m_audioID);
            rtn->m_audio.m_fileName = fmt::format("{}.mp3", rtn->m_audio.m_title);
            rtn->m_audio.m_artistID = rtn->m_artist.m_artistID;
            rtn->customSetup();
            return rtn;
        }
        void customSetup() {
            //form_box
            auto form_box = CCLayer::create();
            if (form_box) {
                //base sets
                form_box->setID("form_box");
                form_box->setPosition(CCPoint(0.f, 116.f));
                form_box->setContentSize(CCSize(255.f, 175.f));
                form_box->ignoreAnchorPointForPosition(false);
                //add
                this->m_buttonMenu->addChild(form_box);
                //layout
                auto layout = ColumnLayout::create();
                layout->setAxisReverse(1);
                form_box->setLayout(layout);
            }
            //addInputs
            auto addInput = [this, form_box](auto label = "", auto id = "", CommonFilter commonFilter = CommonFilter::Any) {
                //labelNode
                auto labelNode = CCLabelBMFont::create(label, "bigFont.fnt");
                labelNode->setID(id + std::string("/label"));
                form_box->addChild(labelNode);
                //inputNode
                auto inputNode = TextInput::create(form_box->getContentWidth(), label, "chatFont.fnt");
                inputNode->getInputNode()->setID(id);
                inputNode->getInputNode()->setMaxLabelScale(1.f);
                inputNode->setCommonFilter(commonFilter);
                inputNode->setDelegate(this);
                form_box->addChild(inputNode);
                //upd
                form_box->updateLayout();
                };
            addInput("Target ID", "m_songID");
            loadInputs();
            //help
            form_box->addChild(CCLabelBMFont::create("Help", "bigFont.fnt"));
            form_box->updateLayout();
            if (auto menu = CCMenu::create()) {
                //layout
                auto layout = RowLayout::create();
                menu->setLayout(layout);
                //Audios List
                menu->addChild(CCMenuItemExt::createSpriteExtra(
                    ButtonSprite::create("Audios List", 0.6f),
                    [](auto) {
                        AudiosListMDPopup::create()->show();
                    }
                ));
                //Settings
                menu->addChild(CCMenuItemExt::createSpriteExtra(
                    ButtonSprite::create("Settings", 0.6f),
                    [](auto) {
                        openSettingsPopup(Mod::get());
                    }
                ));
                //lyupd
                menu->updateLayout();
                //add
                form_box->addChild(menu);
                form_box->updateLayout();
                handleTouchPriority(this);
            }
        }
        void loadInputs() {
            auto m_songID = typeinfo_cast<CCTextInputNode*>(this->getChildByIDRecursive("m_songID"));
            if (m_songID) m_songID->setString(fmt::format("{}", m_audio.m_audioID));
        }
        void applyInputs() {
            auto m_songID = typeinfo_cast<CCTextInputNode*>(this->getChildByIDRecursive("m_songID"));
            if (m_songID) m_audio.m_audioID = utils::numFromString<int>(m_songID->getString().data()).unwrapOr(m_audio.m_audioID);
        }
        void FLAlert_Clicked(FLAlertLayer* p0, bool p1) {
            if (not p1) return;
            applyInputs();
            //save artist
            auto artist_meta_file = artists_meta_path / fmt::format("{}.json", m_artist.m_artistID);
            std::ofstream(artist_meta_file) << m_artist.resetJson().dump(matjson::TAB_INDENTATION);
            //save audio
            auto audio_meta_file = audios_meta_path / fmt::format("{}.json", m_audio.m_audioID);
            std::ofstream(audio_meta_file) << m_audio.resetJson().dump(matjson::TAB_INDENTATION);
            //save music file
            fs::copy(
                MusicDownloadManager::sharedState()->pathForSong(
                    findDataNode(refSongInfoLayer, "m_songID")->getTag()
                ).data(),
                songs_path / m_audio.m_fileName,
                fs::copy_options::overwrite_existing,
                fs::last_err_code
            );
            //call custom func
            this->m_onSave();
        };
    };
}