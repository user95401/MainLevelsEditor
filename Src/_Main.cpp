#include "ModUtils.hpp"
#include "HooksUtils.hpp"
using namespace cocos2d;
using namespace cocos2d::extension;

#include "Layers/CocosHeadersOnlyHookExample.hpp"

HWND hWnd;

DWORD WINAPI ModThread(void* hModule) {
    //game ver check
    if (ModUtils::GetGameVersion() != "2.200")
        if (MessageBoxA(hWnd, "This mod developed for Geometry Dash 2.200!\nStill countinue?", "Wrong game version", MB_OKCANCEL | MB_ICONWARNING) != IDOK)
            return 1;
    //othher stuff
    MH_Initialize();
    MenuLayerSkit::CreateHooks();
    SetWindowTextA(hWnd, ("Geometry Das.. h: Modded by " + ModUtils::GetModDev()).c_str());
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    if (ul_reason_for_call != DLL_PROCESS_ATTACH) return TRUE;
    hWnd = GetForegroundWindow();//сatch the game window right on launch (maybe on launch) hehe
    CreateThread(0, 0, ModThread, hModule, 0, 0);
    return TRUE;
}
