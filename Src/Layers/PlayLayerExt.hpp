#pragma once
#include "HooksUtils.hpp"
#include "ModUtils.hpp"
using namespace cocos2d;
using namespace extension;

using namespace gd;

class PlayLayerExt : public PlayLayer {
public:
    inline static PlayLayerExt* me;
    static void CreateHooks();
    void onSomeBtn(CCObject* pSender);
};

