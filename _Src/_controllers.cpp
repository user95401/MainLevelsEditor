#include <Geode/Geode.hpp>
using namespace geode::prelude;

#include "Header1.hpp"

//i know all like use geode solutions blah blah blah but i dont like them
//here collected all helpers that i selected
#include "SimpleIni.h"
#ifdef GEODE_IS_WINDOWS
#include "patterns.hpp"
#include "ModUtils.hpp"
#endif


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
            if (ghc::filesystem::exists(levelDataPath)) {
                level->m_levelString = read_file(levelDataPath);
            };
        };
        if (level->m_levelType == GJLevelType::Saved) {
            auto levelDataPath = FilePathFromModFolder(fmt::format("levels/{}.txt", level->m_levelID.value()));
            if (ghc::filesystem::exists(levelDataPath)) {
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
        LoadingLayer::loadingFinished();
    };
};

#include <Geode/modify/LevelSelectLayer.hpp>
class $modify(LevelSelectLayer) {
    bool init(int p0) {
        UpdatePagesSetup();
        auto rtn = LevelSelectLayer::init(p0);
        
#ifndef GEODE_IS_WINDOWS
        //BoomScrollLayer* ohfuck = MEMBERBYOFFSET(BoomScrollLayer*, this, 336);
        //ohfuck->setScale(0.8f);
#endif
        return rtn;
    };
    ccColor3B colorForPage(int page) {
        ccColor3B _ccColor3B = LevelSelectLayer::colorForPage(page);

        std::string IniPath = FilePathFromModFolder("_PageColors.ini");
        std::string MainSection = fmt::format("colorForPage");
        std::string MainVal = fmt::format("{}", page);

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
            _ccColor3B.r = atoi(rgb[0].data());
            _ccColor3B.g = atoi(rgb[1].data());
            _ccColor3B.b = atoi(rgb[2].data());
        }

        Ini.SaveFile(IniPath.c_str());
        /**/

        return _ccColor3B;
    }
    /**/
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
        GJGameLevel* pGJGameLevel = processOutLevelByConfig(p0, LevelTools::getLevel(p0, p1));
        return pGJGameLevel;
    }
    static bool verifyLevelIntegrity(gd::string p0, int p1) {
        return 1;
    }
};
/**/
