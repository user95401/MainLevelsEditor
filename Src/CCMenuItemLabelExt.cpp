#include "CCMenuItemLabelExt.hpp"
using namespace cocos2d;

CCMenuItemLabelExt* CCMenuItemLabelExt::create(cocos2d::CCNode* label, cocos2d::CCObject* target, cocos2d::SEL_MenuHandler selector) {
    CCMenuItemLabelExt* pRet = new CCMenuItemLabelExt();
    if (pRet && pRet->initWithLabel(label, target, selector)) { pRet->autorelease(); return pRet; }
    else { delete pRet; pRet = NULL; return NULL; }
}
void CCMenuItemLabelExt::selected() {
    if (this->m_fOriginalSize > 0.001) this->m_fOriginalSize = this->getScale();
    CCScaleTo* pCCScaleTo = CCScaleTo::create(0.300f, m_fOriginalSize * 1.26f);
    CCAction* pScaleAction = CCEaseBounceOut::create(pCCScaleTo);
    this->runAction(pScaleAction);
}
void CCMenuItemLabelExt::unselected() {
    CCScaleTo* pCCScaleTo = CCScaleTo::create(0.300f, m_fOriginalSize);
    CCAction* pScaleAction = CCEaseBounceOut::create(pCCScaleTo);
    this->runAction(pScaleAction);
}