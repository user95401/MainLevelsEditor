#include "CustomLayer.hpp"

//custom button that similar to CCMenuItemSpriteExtra
#include "../CCMenuItemLabelExt.hpp"

bool CustomLayer::init() {
    //allow keypad for layer
    setKeypadEnabled(true);
    //allow touching for layer
    setTouchEnabled(true);
    
    //we love that
    auto winSize = CCDirector::sharedDirector()->getWinSize();

    CCMenu* Menu = CCMenu::create();
    Menu->setPosition(CCPoint());
    addChild(Menu, 100, 5940);//do u know that exists addChild(node, index) and addChild(node, index, tag)???
    
    auto label = CCLabelBMFont::create("Hello world!", "bigFont.fnt");
    label->setPosition(ModUtils::getCenterPoint());
    label->setRotation(12);
    addChild(label);

    CCSprite* backgroundSprite = ModUtils::createSprite("GJ_gradientBG.png");
    backgroundSprite->setScaleX(winSize.width / backgroundSprite->getContentSize().width);
    backgroundSprite->setScaleY(winSize.height / backgroundSprite->getContentSize().height);
    //by default its 0.5, we scalling from corner so we needed 0
    backgroundSprite->setAnchorPoint({ 0, 0 });
    backgroundSprite->setColor({ 0, 102, 255 });//blue color u know
    addChild(backgroundSprite, -1);//zOrder -1 because its bg guys

    auto btnBack = CCMenuItemLabelExt::create(
        ModUtils::createSprite("GJ_arrow_01_001.png"),
        this,
        menu_selector(CustomLayer::onBack)
    );

    auto menuThatExistOnlyForBackBtn = CCMenu::createWithItem(btnBack);
    addChild(menuThatExistOnlyForBackBtn);
    menuThatExistOnlyForBackBtn->setPosition({ 25.000f, winSize.height - 25 });

    //onSomeBtn
    CCMenuItemLabelExt* btn_chatHistory_001 = CCMenuItemLabelExt::create(
        ModUtils::createSprite("btn_chatHistory_001.png"),
        this,
        menu_selector(CustomLayer::onSomeBtn)
    );
    btn_chatHistory_001->setPositionX(winSize.width - 42);
    btn_chatHistory_001->setPositionY(80.000f);
    Menu->addChild(btn_chatHistory_001);

    return true;
}

void CustomLayer::onSomeBtn(CCObject* pSender) {
    //totay we create CRAZY SHIT HERE
    // - notify with powershell
    std::string title = std::format("hi from {}!", ModUtils::GetModDev());
    std::string msg = "thats it, thanks for click on this btn.";
    std::string command(
        "powershell -Command \"& {"
        "Add-Type -AssemblyName System.Windows.Forms; "
        "Add-Type -AssemblyName System.Drawing; "
        "$notify = New-Object System.Windows.Forms.NotifyIcon; "
        "$notify.Icon = [System.Drawing.SystemIcons]::Information; "
        "$notify.Visible = $true; "
        "$notify.ShowBalloonTip(0, '" + title + "', '" + msg + "', [System.Windows.Forms.ToolTipIcon]::None) "
        "}\" "
    );
    //ModUtils::log(command);
    ShellExecuteA(NULL, NULL, "cmd", std::format("/c {}", command).c_str(), NULL, 0);
}

void CustomLayer::onBack(CCObject* object) {
    CCDirector::sharedDirector()->popSceneWithTransition(0.5f, PopTransition::kPopTransitionFade);
}

void CustomLayer::pushToMe(CCObject* pSender) {
    //soo, lets create CCScene with CustomLayer idk what to say else
    //small code king
    auto scene = CCScene::create();
    scene->addChild(CustomLayer::create());
    CCDirector::sharedDirector()->pushScene(CCTransitionFade::create(0.5f, scene));
}

// This is the create method, a static method that creates
// a new instance, and initializes it.
CustomLayer* CustomLayer::create() {
    auto ret = new CustomLayer();
    if (ret && ret->init()) {
        ret->autorelease();
    }
    else {
        // if `new` or `init` went wrong, delete the object and return a nullptr
        delete ret;
        ret = nullptr;
    }
    return ret;
}

//rewritable function that calling when u press escape or any alse back themed events
void CustomLayer::keyBackClicked() {
    onBack(CCNode::create());//or nullptr
}
