#include <Geode/Geode.hpp>
using namespace geode::prelude;

//i know all like use geode solutions blah blah blah but i dont like them
//here collected all helpers that i selected
#include "SimpleIni.h"
#ifdef GEODE_IS_WINDOWS
#include "patterns.hpp"
#include "ModUtils.hpp"
#endif
//they force me

//mindblowing shit 
/////AAAAAAAAAAAAAA I HATE SO MUCHH THAT FU FUCKING GUIDLINES
std::string FilePathFromModFolder(std::string fname) {
    auto sCurrPath = ghc::filesystem::current_path().string();
    auto sFullPath = Mod::get()->getConfigDir(true).string();
    auto sRelPath = sFullPath.erase(0, sCurrPath.size() + 1);//sucks
    std::replace(sRelPath.begin(), sRelPath.end(), '\\', '/'); // replace all '\' to '/' ;3
    std::filesystem::path path = (sRelPath + "/" + fname);
    std::filesystem::create_directories(path.parent_path());
    return path.string();
}
auto read_file(std::string_view path) -> std::string {
    constexpr auto read_size = std::size_t(4096);
    auto stream = std::ifstream(path.data());
    stream.exceptions(std::ios_base::badbit);
    if (not stream) {
        throw std::ios_base::failure("file does not exist");
    }
    auto out = std::string();
    auto buf = std::string(read_size, '\0');
    while (stream.read(&buf[0], read_size)) {
        out.append(buf, 0, stream.gcount());
    }
    out.append(buf, 0, stream.gcount());
    return out;
}

void UpdatePagesSetup() {
#ifdef GEODE_IS_WINDOWS

    int cmp_amount = 23;
    int start_from = 1;

    std::string MainSection = fmt::format("UpdatePagesSetup");
    std::string IniPath = FilePathFromModFolder("_PagesSetupPatch.ini");

    CSimpleIni Ini;
    Ini.LoadFile(IniPath.c_str());

    //cmp_amount
    if (!(Ini.KeyExists(MainSection.c_str(), "cmp_amount")))
        Ini.SetLongValue(
            MainSection.c_str(),
            "cmp_amount",
            cmp_amount,
            "; level pages adding amount (u cant set more than 127!)"
        );
    else cmp_amount = Ini.GetLongValue(MainSection.c_str(), "cmp_amount");

    //start_from
    if (!(Ini.KeyExists(MainSection.c_str(), "start_from")))
        Ini.SetLongValue(
            MainSection.c_str(),
            "start_from",
            start_from,
            "; acts as the first value when adding the rest levels page"
        );
    else start_from = Ini.GetLongValue(MainSection.c_str(), "start_from");

    Ini.SaveFile(IniPath.c_str());

    //we at LevelSelectLayer::init(LevelSelectLayer* pClass, int initLevel)
    //way: "Download the soundtrack" string, move up to sideart sprites

    /*  2.204 findings

    26812C(rva) - while amount
    opcodes: 83 FE 17 7C 9F 8B 5C 24 1C BE 03 00 00 00 8D 9B
    targ:__________^ 17(16)=23(10)
    asm: cmp esi, 0x17

    //2680BF(rva) - start from
    opcodes: be 1 0 0 0 89 44 24 18 8b d8 8d 9b
    targ:_______^ that is 1 we know
    asm: mov esi, 0x01

    */

    {
        //std::vector<unsigned char> toRewrite = intToBytes(cmp_amount);
        auto addr = patterns::find_pattern("83 FE ? 7C 9F 8B 5C 24 1C BE 03 00 00 00 8D 9B");
        WriteProcMem(addr, { 0x83, 0xFE, (uint8_t)cmp_amount });
    }

    {
        std::vector<unsigned char> toRewrite = intToBytes(start_from);
        auto addr = patterns::find_pattern("be ???? 89 44 24 18 8b d8 8d 9b");
        //addr = gd::base + 0x2680BF; //2.204
        //
        // 
        // 
        // 
        // log((ReadProcMemAsStr(addr, 22)).c_str());
        WriteProcMem(addr, { 0xBE, toRewrite[0], toRewrite[1], toRewrite[2], toRewrite[3] });
    }
#endif
}

//huh
#include <Geode/modify/PlayLayer.hpp>
class $modify(PlayLayer) {
    static PlayLayer* create(GJGameLevel * level, bool useReplay, bool dontCreateObjects) {
        if (level->m_levelType == GJLevelType::Local) {
            auto levelDataPath = FilePathFromModFolder(fmt::format("levels/{}.txt", level->m_levelID.value()));
            if (std::filesystem::exists(levelDataPath)) {
                level->m_levelString = read_file(levelDataPath);
            };
        };
        return PlayLayer::create(level, useReplay, dontCreateObjects);
    }
};

#include <Geode/modify/LoadingLayer.hpp>
class $modify(LoadingLayer) {
    TodoReturn loadingFinished() {
        //create some inis
        LevelSelectLayer::create(0);
        //break level str check
        WriteProcMem(patterns::find_pattern("8a c3 5b 5d c3", ""), { 0xB0, 0x01 });
        LoadingLayer::loadingFinished();
    };
};

#include <Geode/modify/LevelSelectLayer.hpp>
class $modify(LevelSelectLayer) {
    bool init(int p0) {
        auto rtn = LevelSelectLayer::init(p0);
        UpdatePagesSetup();
        return rtn;
    };
    ccColor3B colorForPage(int page) {
        ccColor3B _ccColor3B = LevelSelectLayer::colorForPage(page);
        
        std::string MainSection = fmt::format("colorForPage");
        std::string MainVal = fmt::format("{}", page);
        std::string IniPath = FilePathFromModFolder("_PageColors.ini");

        CSimpleIni Ini;
        Ini.LoadFile(IniPath.c_str());

        //Color
        if (!(Ini.KeyExists(MainSection.c_str(), MainVal.c_str())))
            Ini.SetValue(
                MainSection.c_str(),
                MainVal.c_str(),
                fmt::format("{},{},{}", _ccColor3B.r, _ccColor3B.g, _ccColor3B.b).c_str()
            );
        else {
            std::vector<std::string> rgb = explode(Ini.GetValue(MainSection.c_str(), MainVal.c_str()), ',');
            _ccColor3B.r = stoi(rgb[0]);
            _ccColor3B.g = stoi(rgb[1]);
            _ccColor3B.b = stoi(rgb[2]);
        }

        Ini.SaveFile(IniPath.c_str());

        return _ccColor3B;
    }
};

#if 1
#define crRetAAAsdp0(MainValaa, IniName) \
std::string MainSection = fmt::format("{}", p0); \
std::string MainVal = fmt::format("{}", MainValaa); \
std::string IniPath = IniName; \
CSimpleIni Ini; \
Ini.LoadFile(IniPath.c_str()); \
if (!(Ini.KeyExists(MainSection.c_str(), MainVal.c_str()))) \
{Ini.SetValue(MainSection.c_str(),MainVal.c_str(),crRet.c_str());} \
else {crRet = Ini.GetValue(MainSection.c_str(), MainVal.c_str());} \
 \
Ini.SaveFile(IniPath.c_str());
#endif

#include <Geode/modify/LevelTools.hpp>
class $modify(LevelTools) {
    static gd::string getAudioFileName(int p0) {
        std::string crRet = LevelTools::getAudioFileName(p0);
        crRetAAAsdp0("Filename", FilePathFromModFolder("_AudioTracks.ini"));
        return crRet;
    }
    static gd::string getAudioTitle(int p0) {
        gd::string crRet = LevelTools::getAudioTitle(p0);
        crRetAAAsdp0("Title", FilePathFromModFolder("_AudioTracks.ini"));
        return crRet;
    }
    static gd::string nameForArtist(int p0) {
        gd::string crRet = LevelTools::nameForArtist(p0);
        crRetAAAsdp0("name", FilePathFromModFolder("_Artists.ini"));
        return crRet;
    }
    static gd::string fbURLForArtist(int p0) {
        gd::string crRet = LevelTools::fbURLForArtist(p0);
        crRetAAAsdp0("fbURL", FilePathFromModFolder("_Artists.ini"));
        return crRet;
    }
    static gd::string ngURLForArtist(int p0) {
        gd::string crRet = LevelTools::ngURLForArtist(p0);
        crRetAAAsdp0("ngURL", FilePathFromModFolder("_Artists.ini"));
        return crRet;
    }
    static gd::string ytURLForArtist(int p0) {
        gd::string crRet = LevelTools::ytURLForArtist(p0);
        crRetAAAsdp0("ytURL", FilePathFromModFolder("_Artists.ini"));
        return crRet;
    }
    static GJGameLevel* getLevel(int p0, bool p1) {
        GJGameLevel* pGJGameLevel = LevelTools::getLevel(p0, p1);
        //mindblowing shit 
        /*if (pGJGameLevel->m_levelString == LevelTools::getLevel(1, p1)->m_levelString) {
            log::debug("{}: levelstr same as 1", p0);
            return pGJGameLevel;
        }*/
        std::string MainSection = fmt::format("Level Setup");
        std::string IniPath = FilePathFromModFolder(fmt::format("levels/setup/{}.ini", p0));

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

        Ini.SaveFile(IniPath.c_str());

        //CCMessageBox(fmt::format("{}", ).c_str(), __FUNCSIG__);
        return pGJGameLevel;
    }
};