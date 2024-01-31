#include "ModUtils.hpp"
#include "HooksUtils.hpp"
#include <format>
using namespace cocos2d;
using namespace cocos2d::extension;
using namespace gd;

#include "SimpleIni.h"

std::vector<unsigned char> intToBytes(int value) {
    std::vector<unsigned char> result;
    result.push_back(value & 0x000000ff);
    result.push_back((value & 0x0000ff00) >> 8);
    result.push_back((value & 0x00ff0000) >> 16);
    result.push_back((value & 0xff000000) >> 24);
    return result;
}

void UpdatePagesSetup() {

    int cmp_amount = 23;
    int start_from = 1;

    std::string MainSection = std::format("UpdatePagesSetup");
    std::string IniPath = "Resources/levels/settings/_PagesSetupPatch.ini";
    std::filesystem::create_directories("Resources/levels/settings/");

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
    targ:_________________________^ that is 1 we know
    asm: mov esi, 0x01

    */

    {
        //std::vector<unsigned char> toRewrite = intToBytes(cmp_amount);
        auto addr = patterns::find_pattern("83 FE ? 7C 9F 8B 5C 24 1C BE 03 00 00 00 8D 9B");
        ModUtils::WriteProcMem(addr, { 0x83, 0xFE, (uint8_t)cmp_amount });
    }

    {
        std::vector<unsigned char> toRewrite = intToBytes(start_from);
        auto addr = patterns::find_pattern("be ???? 89 44 24 18 8b d8 8d 9b");
        //addr = gd::base + 0x2680BF; //2.204
        //ModUtils::log((ModUtils::ReadProcMemAsStr(addr, 22)).c_str());
        ModUtils::WriteProcMem(addr, { 0xBE, toRewrite[0], toRewrite[1], toRewrite[2], toRewrite[3] });
    }

}

//268AC0(rva)
//android say int __fastcall LevelSelectLayer::colorForPage(LevelSelectLayer *this, int a2)
ccColor3B __fastcall colorForPage(CCLayer *pClass, void* asd, int page) {
    ccColor3B _ccColor3B = MappedHooks::getOriginal(colorForPage)(pClass, asd, page);

    std::string MainSection = std::format("colorForPage");
    std::string MainVal = std::format("{}", page);
    std::string IniPath = "Resources/levels/settings/_PageColors.ini";
    std::filesystem::create_directories("Resources/levels/settings/");

    CSimpleIni Ini;
    Ini.LoadFile(IniPath.c_str());

    //Color
    if (!(Ini.KeyExists(MainSection.c_str(), MainVal.c_str())))
        Ini.SetValue(
            MainSection.c_str(),
            MainVal.c_str(),
            std::format("{},{},{}", _ccColor3B.r, _ccColor3B.g, _ccColor3B.b).c_str()
        );
    else {
        std::vector<std::string> rgb = ModUtils::explode(Ini.GetValue(MainSection.c_str(), MainVal.c_str()), ',');
        _ccColor3B.r = stoi(rgb[0]);
        _ccColor3B.g = stoi(rgb[1]);
        _ccColor3B.b = stoi(rgb[2]);
    }

    Ini.SaveFile(IniPath.c_str());

    return _ccColor3B;
}

std::string __fastcall getAudioFilename(int levelID) {
    std::string crRet = MappedHooks::getOriginal(getAudioFilename)(levelID);

    std::string MainSection = std::format("getAudioFilename");
    std::string MainVal = std::format("{}", levelID);
    std::string IniPath = "Resources/levels/settings/_AudioTracks.ini";
    std::filesystem::create_directories("Resources/levels/settings/");

    CSimpleIni Ini;
    Ini.LoadFile(IniPath.c_str());

    if (!(Ini.KeyExists(MainSection.c_str(), MainVal.c_str())))
        Ini.SetValue(
            MainSection.c_str(),
            MainVal.c_str(),
            crRet.c_str()
        );
    else crRet = Ini.GetValue(MainSection.c_str(), MainVal.c_str());

    Ini.SaveFile(IniPath.c_str());

    return crRet;
}

GJGameLevel* __fastcall getLevel(int levelID, bool huh) {
    GJGameLevel* pGJGameLevel = MappedHooks::getOriginal(getLevel)(levelID, huh);

    std::string MainSection = std::format("Level Setup");
    std::string IniPath = std::format("Resources/levels/settings/{}.ini", levelID);
    std::filesystem::create_directories("Resources/levels/settings/");

    CSimpleIni Ini;
    Ini.LoadFile(IniPath.c_str());

    //m_sLevelName
    if (!(Ini.KeyExists(MainSection.c_str(), "m_sLevelName")))
        Ini.SetValue(
            MainSection.c_str(), 
            "m_sLevelName", 
            pGJGameLevel->m_sLevelName.c_str(),
            "; Level Name"
        );
    else pGJGameLevel->m_sLevelName = Ini.GetValue(MainSection.c_str(), "m_sLevelName");

    //m_difficulty
    if (!(Ini.KeyExists(MainSection.c_str(), "m_difficulty")))
        Ini.SetLongValue(
            MainSection.c_str(), 
            "m_difficulty", 
            (int)pGJGameLevel->m_difficulty,
            "; Difficulties that LevelPage layer supports:\n"
            "; Auto = 0,\n"
            "; Easy = 1,\n"
            "; Normal = 2,\n"
            "; Hard = 3,\n"
            "; Harder = 4,\n"
            "; Insane = 5,\n"
            "; Demon = 6"
        );
    else pGJGameLevel->m_difficulty = (GJDifficulty)Ini.GetLongValue(MainSection.c_str(), "m_difficulty");

    //m_stars
    if (!(Ini.KeyExists(MainSection.c_str(), "m_stars")))
        Ini.SetLongValue(
            MainSection.c_str(), 
            "m_stars", 
            pGJGameLevel->m_stars.value(),
            "; Stars"
        );
    else {
        int stars = Ini.GetLongValue(MainSection.c_str(), "m_stars");
        pGJGameLevel->m_stars.m_value = stars;
        pGJGameLevel->m_stars.m_value_rand = 1000;
        pGJGameLevel->m_stars.m_value_seed = 1000 - stars;
    }

    //m_audioTrack
    if (!(Ini.KeyExists(MainSection.c_str(), "m_audioTrack")))
        Ini.SetLongValue(
            MainSection.c_str(), 
            "m_audioTrack", 
            pGJGameLevel->m_audioTrack,
            "; Audio Track ID"
        );
    else pGJGameLevel->m_audioTrack = Ini.GetLongValue(MainSection.c_str(), "m_audioTrack");

    //m_capacityString
    if (!(Ini.KeyExists(MainSection.c_str(), "m_capacityString")))
        Ini.SetValue(
            MainSection.c_str(), 
            "m_capacityString", 
            pGJGameLevel->m_capacityString.c_str(),
            "; Capacity String"
        );
    else pGJGameLevel->m_capacityString = Ini.GetValue(MainSection.c_str(), "m_capacityString");

    Ini.SaveFile(IniPath.c_str());

    //CCMessageBox(std::format("{}", ).c_str(), __FUNCSIG__);
    return pGJGameLevel;
}

DWORD WINAPI ModThread(void* hModule) {
    //Hooks
    MH_Initialize();
    {
        //
        //55 8b ec 6a ff 68 58 cc b9 0 64 a1 0 0 0 0 50 83 ec 54 53 56//2.204
        //55 8b ec 6a ff 68 58 cc 91 0 64 a1 0 0 0 0 50 83 ec 54 53 56//2.204 after day...
        //55 8b ec 6a ff 68 58 cc 2e 1 64 a1 0 0 0 0 50 83 ec 54 53 56//2.204 after other day...
        //55 8b ec 6a ff 68 58 cc 2e 1 64 a1 0 0 0 0 50 83 ec 54 53 56 57 a1 0 a0 3f 1 33 c5 50 8d//2.204 after other day but more...
        //e9 f3 e9 9e 2  68 58 cc 2e 1 64 a1 0 0 0 0 50 83 ec 54 53 56 57 a1 0 a0 3f 1 33 c5 50 8d//2.204 hooked
        // 
        // geod
        // 55 8b ec 6a ff 68 58 cc 15 1 64 a1 0 0 0 0 50 83 ec 54 53 56 57 a1 0 a0 26 1 33 c5 50 8d
        // e9 f3 e9 5e 3  68 58 cc 15 1 64 a1 0 0 0 0 50 83 ec 54 53 56 57 a1 0 a0 26 1 33 c5 50 8d//hooked
        // 
        //55 8b ec 6a ff 68 38 5f b2 0 64 a1 0 0 0 0 50 83 ec 54 53 56//2.200

        uintptr_t addr = patterns::find_pattern("? ? ? ? ? 68 58 cc ? ? 64 a1 0 0 0 0 50 83 ec 54 53 56 57 a1 0");
        //addr = gd::base + 0x2725d0; //2.204
        //addr = gd::base + 0x26DDA0; //2.200
        //ModUtils::log((ModUtils::ReadProcMemAsStr(addr, 32)).c_str());
        MappedHooks::registerHook(addr, getLevel);
        //ModUtils::log((ModUtils::ReadProcMemAsStr(addr, 32)).c_str());
    };
    {
        //55 8b ec 56 8b 75 c 57 8b f9 85 f6 74 35 b8 e9 a2 8b 2e 8b ce f7//2.204
        uintptr_t addr = patterns::find_pattern("?????? c 57 8b f9 85 f6 74 35");
        //addr = gd::base + 0x268AC0; //2.204
        //ModUtils::log((ModUtils::ReadProcMemAsStr(addr, 22)).c_str());
        MappedHooks::registerHook(addr, colorForPage);
    };
    {
        //55 8b ec 51 c7 45 fc 0 0 0 0 56 8b f1 83 fa 27 f 87 af 2 0//2.204
        uintptr_t addr = patterns::find_pattern("?????? fc 0 0 0 0 56 8b f1 83 fa 27 f 87 af 2 0");
        //addr = gd::base + 0x273430; //2.204
        //ModUtils::log((ModUtils::ReadProcMemAsStr(addr, 22)).c_str());
        MappedHooks::registerHook(addr, getAudioFilename);
    };
    //Updates
    while (true) {
        UpdatePagesSetup();
        Sleep(100);
    }
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    if (ul_reason_for_call != DLL_PROCESS_ATTACH) return TRUE;
    ModUtils::OpenConsole();
    DisableThreadLibraryCalls(hModule);
    CreateThread(0, 0, ModThread, hModule, 0, 0);
    return TRUE;
}
