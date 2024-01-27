#include "ModUtils.hpp"
#include "HooksUtils.hpp"
#include <format>
using namespace cocos2d;
using namespace cocos2d::extension;
using namespace gd;

void UpdateLevelSelectLayerCount() {

}

#include "SimpleIni.h"
//0x2725d0
GJGameLevel* __fastcall getLevel(int lid, bool idk) {
    GJGameLevel* pGJGameLevel = MappedHooks::getOriginal(getLevel)(lid, idk);

    std::string MainSection = std::format("Level Setup");
    std::string IniPath = std::format("Resources/levels/{}.txt", lid);
    std::filesystem::create_directories("Resources/levels/");

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
    MH_Initialize();
    //55 8b ec 6a ff 68 58 cc b9 0 64 a1 0 0 0 0 50 83 ec 54 53 56//2.204
    //55 8b ec 6a ff 68 38 5f b2 0 64 a1 0 0 0 0 50 83 ec 54 53 56//2.200
    uintptr_t addr = patterns::find_patterns("??????? cc b9 0 64 a1 0 0 0 0 50 83 ec 54 53 56")[0];
    //addr = gd::base + 0x2725d0; //2.204
    //addr = gd::base + 0x26DDA0; //2.200
    ModUtils::log((ModUtils::ReadProcMemAsStr(addr, 22)).c_str());
    MappedHooks::registerHook(addr, getLevel);
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    if (ul_reason_for_call != DLL_PROCESS_ATTACH) return TRUE;
    ModUtils::OpenConsole();
    DisableThreadLibraryCalls(hModule);
    CreateThread(0, 0, ModThread, hModule, 0, 0);
    return TRUE;
}
