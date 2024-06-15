#include <Geode/Geode.hpp>
using namespace geode::prelude;

#include "Header1.hpp"

#include "SimpleIni.h"

#include <regex>

//mindblowing shit 
/////AAAAAAAAAAAAAA I HATE SO MUCHH THAT FU FUCKING GUIDLINES
std::string FilePathFromModFolder(std::string fname) {//they force me
    auto sFullPath = Mod::get()->getConfigDir(true).string();
#ifdef GEODE_IS_WINDOWS
    auto sCurrPath = std::filesystem::current_path().string();
    auto sRelPath = sFullPath.erase(0, sCurrPath.size() + 1);//sucks
    std::filesystem::path path = (sRelPath + "//" + fname);
    std::filesystem::create_directories(path.parent_path());
#else
    std::filesystem::path path = (sFullPath + "//" + fname);
#endif
    std::filesystem::create_directories(path.parent_path());
    auto rtn = path.string();
    //std::replace(rtn.begin(), rtn.end(), '\\', '/'); // replace all '\' to '/' ;3
    if(rtn.find("\\_") != std::string::npos) rtn.insert(rtn.find("\\_"), "\\"); // fun thing (MDLNK)
    //log::debug("{}({}): {}", __func__, fname, rtn);
    return rtn;
}

auto read_file(std::string_view path) -> std::string {
    constexpr auto read_size = std::size_t(4096);
    auto stream = std::ifstream(path.data());
    stream.exceptions(std::ios_base::badbit);
    if (not stream) {
        return fmt::format("file does not exist ({}({}) error)", __func__, path);
    }
    auto out = std::string();
    auto buf = std::string(read_size, '\0');
    while (stream.read(&buf[0], read_size)) {
        out.append(buf, 0, stream.gcount());
    }
    out.append(buf, 0, stream.gcount());
    return out;
}

std::vector<std::string> explode(const std::string& str, const char& ch) {
    std::string next;
    std::vector<std::string> result;
    // For each character in the string
    for (std::string::const_iterator it = str.begin(); it != str.end(); it++) {
        // If we've hit the terminal character
        if (*it == ch) {
            // If we have some characters accumulated
            if (!next.empty()) {
                // Add them to the result vector
                result.push_back(next);
                next.clear();
            }
        }
        else {
            // Accumulate the next character into the sequence
            next += *it;
        }
    }
    if (!next.empty())
        result.push_back(next);
    return result;
}

GJGameLevel* processOutLevelByConfig(int id, GJGameLevel* pGJGameLevel) {

    std::string MainSection = fmt::format("Level Setup");
    std::string IniPath = FilePathFromModFolder(fmt::format("levels/setup/{}.ini", id));
    
    CSimpleIni Ini;
    Ini.LoadFile(IniPath.c_str());

    //m_sLevelName
    if (!(Ini.KeyExists(MainSection.c_str(), "LevelName")))
        Ini.SetValue(
            MainSection.c_str(),
            "LevelName",
            pGJGameLevel->m_levelName.c_str(),
            "; Level Name"
        );
    else pGJGameLevel->m_levelName = Ini.GetValue(MainSection.c_str(), "LevelName");

    //m_difficulty
    if (!(Ini.KeyExists(MainSection.c_str(), "difficulty")))
        Ini.SetLongValue(
            MainSection.c_str(),
            "difficulty",
            (int)pGJGameLevel->m_difficulty,
            "; Difficulties that LevelPage layer supports:\n"
            "; undef = 0,\n"
            "; Easy = 1,\n"
            "; Normal = 2,\n"
            "; Hard = 3,\n"
            "; Harder = 4,\n"
            "; Insane = 5,\n"
            "; Demon = 6"
        );
    else pGJGameLevel->m_difficulty = (GJDifficulty)Ini.GetLongValue(MainSection.c_str(), "difficulty");

    //demonDifficulty
    if (!(Ini.KeyExists(MainSection.c_str(), "demonDifficulty")))
        Ini.SetLongValue(
            MainSection.c_str(),
            "demonDifficulty",
            (int)pGJGameLevel->m_demonDifficulty,
            "; idk lol"
        );
    else pGJGameLevel->m_demonDifficulty = Ini.GetLongValue(MainSection.c_str(), "demonDifficulty");
    
    //ratings
    if (!(Ini.KeyExists(MainSection.c_str(), "m_ratings")))
        Ini.SetLongValue(
            MainSection.c_str(),
            "ratings",
            (int)pGJGameLevel->m_ratings,
            "; idk lol"
        );
    else pGJGameLevel->m_ratings = Ini.GetLongValue(MainSection.c_str(), "ratings");

    //m_stars
    if (!(Ini.KeyExists(MainSection.c_str(), "stars")))
        Ini.SetLongValue(
            MainSection.c_str(),
            "stars",
            pGJGameLevel->m_stars.value(),
            "; Stars"
        );
    else {
        int stars = Ini.GetLongValue(MainSection.c_str(), "stars");
        if (Mod::get()->getSettingValue<bool>("SGG")) {
            int max = Mod::get()->getSettingValue<int64_t>("SGGS");
            if (stars >= max) stars = max;
        }
        pGJGameLevel->m_stars = stars;
    }

    //m_audioTrack
    if (!(Ini.KeyExists(MainSection.c_str(), "audioTrack")))
        Ini.SetLongValue(
            MainSection.c_str(),
            "audioTrack",
            pGJGameLevel->m_audioTrack,
            "; Audio Track ID"
        );
    else pGJGameLevel->m_audioTrack = Ini.GetLongValue(MainSection.c_str(), "audioTrack");

    //songID
    if (!(Ini.KeyExists(MainSection.c_str(), "songID")))
        Ini.SetLongValue(
            MainSection.c_str(),
            "songID",
            pGJGameLevel->m_songID,
            "; songID"
        );
    else pGJGameLevel->m_songID = Ini.GetLongValue(MainSection.c_str(), "songID");

    //m_capacityString
    if (!(Ini.KeyExists(MainSection.c_str(), "capacityString")))
        Ini.SetValue(
            MainSection.c_str(),
            "capacityString",
            pGJGameLevel->m_capacityString.c_str(),
            "; Capacity String"
        );
    else pGJGameLevel->m_capacityString = Ini.GetValue(MainSection.c_str(), "capacityString");

    //m_levelDesc
    if (!(Ini.KeyExists(MainSection.c_str(), "levelDesc")))
        Ini.SetValue(
            MainSection.c_str(),
            "levelDesc",
            pGJGameLevel->m_levelDesc.c_str(),
            "; m_levelDesc (useless)"
        );
    else pGJGameLevel->m_levelDesc = Ini.GetValue(MainSection.c_str(), "levelDesc");

    //m_creatorName
    if (!(Ini.KeyExists(MainSection.c_str(), "creatorName")))
        Ini.SetValue(
            MainSection.c_str(),
            "creatorName",
            pGJGameLevel->m_creatorName.c_str(),
            "; m_creatorName (useless)"
        );
    else pGJGameLevel->m_creatorName = Ini.GetValue(MainSection.c_str(), "creatorName");

    //m_levelLength
    if (!(Ini.KeyExists(MainSection.c_str(), "levelLength")))
        Ini.SetLongValue(
            MainSection.c_str(),
            "levelLength",
            pGJGameLevel->m_levelLength,
            "; m_levelLength (useless)"
        );
    else pGJGameLevel->m_levelLength = Ini.GetLongValue(MainSection.c_str(), "levelLength");

    //m_featured
    if (!(Ini.KeyExists(MainSection.c_str(), "featured")))
        Ini.SetLongValue(
            MainSection.c_str(),
            "featured",
            pGJGameLevel->m_featured,
            "; m_featured (useless)"
        );
    else pGJGameLevel->m_featured = Ini.GetLongValue(MainSection.c_str(), "featured");

    //m_isEpic
    if (!(Ini.KeyExists(MainSection.c_str(), "isEpic")))
        Ini.SetLongValue(
            MainSection.c_str(),
            "isEpic",
            pGJGameLevel->m_isEpic,
            "; m_isEpic (useless)"
        );
    else pGJGameLevel->m_isEpic = Ini.GetLongValue(MainSection.c_str(), "isEpic");

    //m_autoLevel
    if (!(Ini.KeyExists(MainSection.c_str(), "autoLevel")))
        Ini.SetLongValue(
            MainSection.c_str(),
            "autoLevel",
            pGJGameLevel->m_autoLevel,
            "; m_autoLevel (useless)"
        );
    else pGJGameLevel->m_autoLevel = Ini.GetLongValue(MainSection.c_str(), "autoLevel");

    //m_levelType
    if (!(Ini.KeyExists(MainSection.c_str(), "levelType")))
        Ini.SetLongValue(
            MainSection.c_str(),
            "levelType",
            (int)pGJGameLevel->m_levelType,
            "; m_levelType (useless)"
            "\n; " " Local = 1"
            "\n; " "Editor = 2"
            "\n; " " Saved = 3"
        );
    else pGJGameLevel->m_levelType = (GJLevelType)Ini.GetLongValue(MainSection.c_str(), "levelType");

    //m_songIDs
    if (!(Ini.KeyExists(MainSection.c_str(), "songIDs")))
        Ini.SetValue(
            MainSection.c_str(),
            "songIDs",
            pGJGameLevel->m_songIDs.data(),
            "; songIDs"
        );
    else pGJGameLevel->m_songIDs = Ini.GetValue(MainSection.c_str(), "songIDs");

    //sfxIDs
    if (!(Ini.KeyExists(MainSection.c_str(), "sfxIDs")))
        Ini.SetValue(
            MainSection.c_str(),
            "sfxIDs",
            pGJGameLevel->m_sfxIDs.data(),
            "; m_sfxIDs (useless)"
        );
    else pGJGameLevel->m_sfxIDs = Ini.GetValue(MainSection.c_str(), "sfxIDs");

    Ini.SaveFile(IniPath.c_str());

    return pGJGameLevel;
}

std::string saveToMainLevel(int id, GJGameLevel* pGJGameLevel) {
    auto resultstr = gd::string("The <cy>level</c> was <co>copied</c>...");
    //grab level
    GJGameLevel* pLevel = pGJGameLevel;
    {
        std::string MainSection = fmt::format("Level Setup");
        std::string IniPath = FilePathFromModFolder(fmt::format("levels/setup/{}.ini", id));

        CSimpleIni Ini;
        Ini.LoadFile(IniPath.c_str());

        //m_sLevelName
        Ini.SetValue(
            MainSection.c_str(),
            "LevelName",
            pGJGameLevel->m_levelName.c_str(),
            "; Level Name"
        );

        //m_difficulty
        auto difficulty = pGJGameLevel->m_starsRequested;
        switch (pGJGameLevel->m_starsRequested) {
        case 1: difficulty = 1; break;
        case 2: difficulty = 1; break;
        case 3: difficulty = 2; break;
        case 4: difficulty = 3; break;
        case 5: difficulty = 3; break;
        case 6: difficulty = 4; break;
        case 7: difficulty = 4; break;
        case 8: difficulty = 5; break;
        case 9: difficulty = 5; break;
        case 10: difficulty = 6; break;
        default: difficulty = 1; break;
        };
        Ini.SetLongValue(
            MainSection.c_str(),
            "difficulty",
            difficulty,
            "; Difficulties that LevelPage layer supports:\n"
            "; undef = 0,\n"
            "; Easy = 1,\n"
            "; Normal = 2,\n"
            "; Hard = 3,\n"
            "; Harder = 4,\n"
            "; Insane = 5,\n"
            "; Demon = 6"
        );

        //m_stars
        auto stars = pGJGameLevel->m_starsRequested == 0 ? pGJGameLevel->m_stars : pGJGameLevel->m_starsRequested;
        Ini.SetLongValue(
            MainSection.c_str(),
            "stars",
            stars,
            "; Stars"
        );

        //m_audioTrack
        auto audioTrack = (pLevel->m_audioTrack == 0) ? pLevel->m_songID : pLevel->m_audioTrack;
        Ini.SetLongValue(
            MainSection.c_str(),
            "audioTrack",
            audioTrack,
            "; Audio Track ID"
        );

        //m_capacityString
        Ini.SetValue(
            MainSection.c_str(),
            "capacityString",
            pGJGameLevel->m_capacityString.c_str(),
            "; Capacity String"
        );

        //m_levelDesc
        Ini.SetValue(
            MainSection.c_str(),
            "levelDesc",
            pGJGameLevel->m_levelDesc.c_str(),
            "; m_levelDesc (useless)"
        );

        //m_creatorName
        Ini.SetValue(
            MainSection.c_str(),
            "creatorName",
            pGJGameLevel->m_creatorName.c_str(),
            "; m_creatorName (useless)"
        );

        Ini.SaveFile(IniPath.c_str());
    };
    //data store
    {
        //levelString
        std::string level_data = pGJGameLevel->m_levelString;
        std::string decompressed = ZipUtils::decompressString(level_data, false, 0);
        //user coin id 1329, 142 golden coin
        decompressed = std::regex_replace(decompressed, std::regex(",1329,"), ",142,");
        level_data = ZipUtils::compressString(decompressed, false, 0);
        //save
        std::string levelDataPath = FilePathFromModFolder(fmt::format("levels/{}.txt", id));
        std::ofstream levelData;
        levelData.open(levelDataPath);
        levelData.clear();
        levelData << level_data.data();
        levelData.close();
    };
    //_PagesSetupPatch
    {
        std::string MainSection = fmt::format("{}", "UpdatePagesSetup");
        std::string IniPath = FilePathFromModFolder("_PagesSetupPatch.ini");

        CSimpleIni Ini;
        Ini.LoadFile(IniPath.c_str());

        int tar = id;
        if (tar > Ini.GetLongValue(MainSection.c_str(), "cmp_amount") and tar < 128) {
            Ini.SetLongValue(
                MainSection.c_str(),
                "cmp_amount",
                tar,
                "; level pages adding amount (u cant set more than 127!)"
            );
        }
        Ini.SaveFile(IniPath.c_str());
    };
    //_AudioTracks
    if (pLevel->m_audioTrack <= 0) {
        auto newSongPath = FilePathFromModFolder(fmt::format("audio/{} ({}).mp3", pLevel->m_levelName.c_str(), pLevel->m_songID));

        std::ifstream src(pLevel->getAudioFileName(), std::ios::binary);
        std::ofstream dst(newSongPath, std::ios::binary);
        dst << src.rdbuf();

        if (!checkExistence(pLevel->getAudioFileName().data())) {
            resultstr = resultstr.data() + std::string("\n<cr>song wasn't downloaded!</c>\n<co>download it first and retry...</c>");
        }

        std::string MainSection = fmt::format("{}", pLevel->m_songID);
        std::string IniPath = FilePathFromModFolder("_AudioTracks.ini");

        CSimpleIniA Ini;
        Ini.LoadFile(IniPath.c_str());

        Ini.SetValue(MainSection.c_str(), "Title", pLevel->m_levelName.c_str());
        Ini.SetValue(MainSection.c_str(), "Filename", newSongPath.c_str());

        Ini.SaveFile(IniPath.c_str());
    };
    //set result string for text area
    return resultstr;
};
void saveLevelData(GJGameLevel* pGJGameLevel) {
    GJGameLevel* pLevel = pGJGameLevel;
    //data store
    {
        //levelString
        std::string level_data = pGJGameLevel->m_levelString;
        std::string decompressed = ZipUtils::decompressString(level_data, false, 0);
        //user coin id 1329, 142 golden coin
        decompressed = std::regex_replace(decompressed, std::regex(",1329,"), ",142,");
        level_data = ZipUtils::compressString(decompressed, false, 0);
        //save
        std::string levelDataPath = FilePathFromModFolder(fmt::format("levels/{}.txt", pGJGameLevel->m_levelID.value()));
        std::ofstream levelData;
        levelData.open(levelDataPath);
        levelData.clear();
        levelData << level_data.data();
        levelData.close();
    };
}

std::string truncate(std::string str, size_t width, bool show_ellipsis)
{
    if (str.length() > width)
        if (show_ellipsis)
            return str.substr(0, width) + "...";
        else
            return str.substr(0, width);
    return str;
}

std::string framePath(CCNode* node) {
    if (auto textureProtocol = typeinfo_cast<CCTextureProtocol*>(node)) {
        if (auto texture = textureProtocol->getTexture()) {
            auto* cachedTextures = CCTextureCache::sharedTextureCache()->m_pTextures;
            for (auto [key, obj] : CCDictionaryExt<std::string, CCTexture2D*>(cachedTextures)) {
                if (obj == texture) {
                    return key.c_str();
                    break;
                }
            }

            if (auto spriteNode = typeinfo_cast<CCSprite*>(node)) {
                auto* cachedFrames = CCSpriteFrameCache::sharedSpriteFrameCache()->m_pSpriteFrames;
                const auto rect = spriteNode->getTextureRect();
                for (auto [key, frame] : CCDictionaryExt<std::string, CCSpriteFrame*>(cachedFrames)) {
                    if (frame->getTexture() == texture && frame->getRect() == rect) {
                        return key.c_str();
                        break;
                    }
                }
            }

        }
    }
    return "nah";
}