#pragma once
#include "ModUtils.hpp"

class CCMenuItemLabelExt : public cocos2d::CCMenuItemLabel {
public:
    float m_fOriginalSize;
    static CCMenuItemLabelExt* create(cocos2d::CCNode* label, cocos2d::CCObject* target, cocos2d::SEL_MenuHandler selector);
    virtual void selected();
    virtual void unselected();
};
