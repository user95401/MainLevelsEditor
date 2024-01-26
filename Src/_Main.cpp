#include "ModUtils.hpp"
#include "HooksUtils.hpp"
using namespace cocos2d;
using namespace cocos2d::extension;
using namespace gd;

#include "Layers/CocosHeadersOnlyHookExample.hpp"
#include "Layers/PlayLayerExt.hpp"

DWORD WINAPI ModThread(void* hModule) {
    MH_Initialize();
    MenuLayerSkit::CreateHooks();
    //PlayLayerExt::CreateHooks();
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    if (ul_reason_for_call != DLL_PROCESS_ATTACH) return TRUE;
    //ModUtils::OpenConsole();
    DisableThreadLibraryCalls(hModule);
    CreateThread(0, 0, ModThread, hModule, 0, 0);
    return TRUE;
}
