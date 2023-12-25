#pragma once
#include "ModUtils.hpp"
using namespace cocos2d;
using namespace extension;
using namespace gd;

class CustomLayer : public CCLayer {
public:
    inline static CustomLayer* me;//why
    CustomLayer* create();
    virtual bool init();
    virtual void keyBackClicked();
    void pushToMe(CCObject*);
    void onBack(CCObject*);
    void onSomeBtn(CCObject*);
};
