#include <Geode/Geode.hpp>//Include the Geode headers.
#include <Geode/utils/web.hpp>//Include the Geode headers.
using namespace geode::prelude;//cocos2d and all Geode namespaces

#include "ModUtils.hpp"
#include "SimpleIni.h"

#include <Geode/cocos/support/zip_support/ZipUtils.h>
#include <regex>
FLAlertLayer* pFLAlertLayer;
InputNode* pTextInput;
GJGameLevel* pLevel;
class GrabLevelPopupProtocol : public FLAlertLayerProtocol {
public:
    static GrabLevelPopupProtocol* create() {
        GrabLevelPopupProtocol* pRet = new GrabLevelPopupProtocol();
        if (pRet) {
            return pRet;
        }
        else {
            CC_SAFE_DELETE(pRet);
            return nullptr;
        }
    }
    virtual void FLAlert_Clicked(FLAlertLayer* p0, bool p1) {
        if (!p1) return;
        auto resultstr = gd::string("The <cy>level</c> was <co>copied</c>...");
        auto loadingstr = gd::string("<cr>adding...</c>");
        std::string strNewLevel = "New level added:";
        auto pop = FLAlertLayer::create("Grab Level:", loadingstr, "OK");
        //dont animate
        pop->m_noElasticity = 1;
        //sync bg
        pop->m_mainLayer->getChildByID("background")->setContentSize(pFLAlertLayer->m_mainLayer->getChildByID("background")->getContentSize());
        //btn1
        pop->m_button1->m_BGSprite->setSpriteFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("geode.loader/baseEditor_Normal_DarkGray.png"));
        pop->m_button1->m_BGSprite->setContentSize(pop->m_button1->getContentSize());
        pop->m_button1->m_BGSprite->setPositionY(pop->m_button1->m_BGSprite->getPositionY() - 1);
        //lets start the show
        pop->show();
        //grab level
        GJGameLevel* pGJGameLevel = pLevel;
        {
            std::string MainSection = std::format("Level Setup");
            std::string IniPath = std::format("Resources/levels/settings/{}.ini", pTextInput->getString());
            std::filesystem::create_directories("Resources/levels/settings/");

            CSimpleIni Ini;
            Ini.LoadFile(IniPath.c_str());

            strNewLevel = std::format("{}\n""IniPath: {}", strNewLevel, IniPath);

            //m_sLevelName
            Ini.SetValue(
                MainSection.c_str(),
                "m_sLevelName",
                pGJGameLevel->m_levelName.c_str(),
                "; Level Name"
            );
            strNewLevel = std::format("{}\n""Name: {}", strNewLevel, pGJGameLevel->m_levelName.data());

            //m_difficulty
            auto difficulty = pGJGameLevel->m_starsRequested;
            switch (pGJGameLevel->m_starsRequested) {
                case 1: difficulty = 1; break;
                case 2: difficulty = 1; break;
                case 3: difficulty = 2; break;
                case 4: difficulty = 3; break;
                case 5: difficulty = 3; break;
                case 6: difficulty = 4; break;
                case 7: difficulty = 4; break;
                case 8: difficulty = 5; break;
                case 9: difficulty = 5; break;
                case 10: difficulty = 6; break;
                default: difficulty = 1; break;
            };
            Ini.SetLongValue(
                MainSection.c_str(),
                "m_difficulty",
                difficulty,
                "; Difficulties that LevelPage layer supports:\n"
                "; undef = 0,\n"
                "; Easy = 1,\n"
                "; Normal = 2,\n"
                "; Hard = 3,\n"
                "; Harder = 4,\n"
                "; Insane = 5,\n"
                "; Demon = 6"
            );
            strNewLevel = std::format("{}\n""Difficulty: {}", strNewLevel, difficulty);

            //m_stars
            auto stars = pGJGameLevel->m_starsRequested;
            Ini.SetLongValue(
                MainSection.c_str(),
                "m_stars",
                stars,
                "; Stars"
            );
            strNewLevel = std::format("{}\n""Stars: {}", strNewLevel, stars);

            //m_audioTrack
            auto audioTrack = (pLevel->m_audioTrack <= 0) ? stoi(pTextInput->getString()) : pLevel->m_audioTrack;
            Ini.SetLongValue(
                MainSection.c_str(),
                "m_audioTrack",
                audioTrack,
                "; Audio Track ID"
            );
            strNewLevel = std::format("{}\n""Audio Track ID: {}", strNewLevel, audioTrack);

            //m_capacityString
            Ini.SetValue(
                MainSection.c_str(),
                "m_capacityString",
                pGJGameLevel->m_capacityString.c_str(),
                "; Capacity String"
            );
            //strNewLevel = std::format("{}\n""Capacity String: {}", strNewLevel, pGJGameLevel->m_capacityString.data());

            Ini.SaveFile(IniPath.c_str());
        };
        //data store
        {
            //levelString
            std::string level_data = pGJGameLevel->m_levelString;
            std::string decompressed = ZipUtils::decompressString(level_data, false, 0);
            //user coin id 1329, 142 golden coin
            decompressed = std::regex_replace(decompressed, std::regex(",1329,"), ",142,");
            level_data = ZipUtils::compressString(decompressed, false, 0);
            //save
            std::string levelDataPath = std::format("Resources/levels/{}.txt", pTextInput->getString());
            std::ofstream levelData;
            levelData.open(levelDataPath);
            levelData.clear();
            levelData << level_data.data();
            levelData.close();
            strNewLevel = std::format(
                "{}"
                "\n"
                "\n""Level string filtered (for user coins => secret coins)"
                "\n""and installed in \"{}\"",
                strNewLevel, 
                levelDataPath
            );
        };
        //_PagesSetupPatch
        {
            std::string MainSection = std::format("UpdatePagesSetup");
            std::string IniPath = "Resources/levels/settings/_PagesSetupPatch.ini";
            std::filesystem::create_directories("Resources/levels/settings/");

            CSimpleIni Ini;
            Ini.LoadFile(IniPath.c_str());

            int tar = std::stoi(pTextInput->getString()) + 1;
            if (tar > Ini.GetLongValue(MainSection.c_str(), "cmp_amount") and tar < 128) {
                Ini.SetLongValue(
                    MainSection.c_str(),
                    "cmp_amount",
                    tar,
                    "; level pages adding amount (u cant set more than 127!)"
                );
            }
            Ini.SaveFile(IniPath.c_str());
            strNewLevel = std::format(
                "{}"
                "\n"
                "\n""{} updated:"
                "\n""cmp_amount = {}", 
                strNewLevel, 
                IniPath, 
                tar
            );
        };
        //_AudioTracks
        if(pLevel->m_audioTrack <= 0) {
            auto newSongPath = std::format("Resources/levels/audio/{} ({}).mp3", pLevel->m_levelName.c_str(), pLevel->m_songID);
            std::filesystem::create_directories("Resources/levels/audio/");

            std::filesystem::create_directories("Resources/songs/custom/");
            std::ifstream src(pLevel->getAudioFileName(), std::ios::binary);
            std::ofstream dst(newSongPath, std::ios::binary);
            dst << src.rdbuf();

            if (!std::filesystem::exists(pLevel->getAudioFileName().data())) {
                resultstr = resultstr.data() + std::string("\n<cr>song wasn't downloaded!</c>\n<co>download it first and retry...</c>");
            }

            std::string MainSection = std::format("{}", pTextInput->getString());
            std::string IniPath = "Resources/levels/settings/_AudioTracks.ini"; 
            std::filesystem::create_directories("Resources/levels/settings/"); 

            CSimpleIniA Ini; 
            Ini.LoadFile(IniPath.c_str()); 

            Ini.SetValue(MainSection.c_str(), "Title", pLevel->m_levelName.c_str());
            Ini.SetValue(MainSection.c_str(), "Filename", newSongPath.c_str());
            
            Ini.SaveFile(IniPath.c_str());
            strNewLevel = std::format(
                "{}"
                "\n"
                "\n""{} added section:"
                "\n""[{}]"
                "\n""Title = {}"
                "\n""Filename = {}",
                strNewLevel,
                IniPath,
                MainSection,
                pLevel->m_levelName.c_str(),
                newSongPath
            );
        };
        //set result string for text area
        dynamic_cast<TextArea*>(pop->getChildByIDRecursive("content-text-area"))->setString(resultstr);
        //bg again
        CCSprite* groundSquareShadow_001 = CCSprite::createWithSpriteFrameName("groundSquareShadow_001.png");
        groundSquareShadow_001->setAnchorPoint({ 0.f, 0.f });
        groundSquareShadow_001->setScale(CCDirector::sharedDirector()->getWinSize().height / groundSquareShadow_001->getContentSize().height);
        pop->addChild(groundSquareShadow_001, -2);
        //huh 
        auto infNewLevel = CCLabelTTF::create(strNewLevel.data(), "arial", 16.f);
        infNewLevel->setHorizontalAlignment(CCTextAlignment::kCCTextAlignmentLeft);
        infNewLevel->setAnchorPoint({ 0.f, 0.f });
        infNewLevel->setScale(0.45f);
        pop->addChild(infNewLevel);
    };
};
class GrabLevelPopup : public FLAlertLayer {
public:
    static FLAlertLayer* create() {
        CSimpleIni Ini;
        Ini.LoadFile("Resources/levels/settings/_PagesSetupPatch.ini");
        int cmp_amount = Ini.GetLongValue("UpdatePagesSetup", "cmp_amount");
        auto cmp_amount_str = Ini.GetValue("UpdatePagesSetup", "cmp_amount");
        auto retn = FLAlertLayer::create(GrabLevelPopupProtocol::create(), "Grab Level:", gd::string(""), "Close", "Grab It!");
        //textInput
        pTextInput = InputNode::create(160.f, "main level id", "chatFont.fnt", "1234567890", 4);
        pTextInput->setPositionY(44.f);
        pTextInput->setString(cmp_amount_str);
        retn->m_buttonMenu->addChild(pTextInput);
        //bg
        CCSprite* groundSquareShadow_001 = CCSprite::createWithSpriteFrameName("groundSquareShadow_001.png");
        groundSquareShadow_001->setAnchorPoint({ 0.f, 0.f });
        groundSquareShadow_001->setScale(CCDirector::sharedDirector()->getWinSize().height / groundSquareShadow_001->getContentSize().height);
        retn->addChild(groundSquareShadow_001, -2);
        //levels list
        std::string strList = "levels list:";
        for (int i = 1; i < 9999; i++) {
            if (std::filesystem::exists(std::format("Resources/levels/{}.txt", i))) {
                std::string levelName = LevelTools::getLevel(i, 0)->m_levelName;
                strList = std::format("{}\n{}: {}", strList, i, levelName);
            };
        }
        auto levelsList = CCLabelTTF::create(strList.data(), "arial", 16.f);
        levelsList->setHorizontalAlignment(CCTextAlignment::kCCTextAlignmentLeft);
        levelsList->setScale(CCDirector::sharedDirector()->getWinSize().height / levelsList->getContentSize().height);
        levelsList->setAnchorPoint({ 0.f, 0.f });
        levelsList->setPositionX(-groundSquareShadow_001->getContentSize().width);
        levelsList->runAction(CCEaseExponentialOut::create(CCMoveTo::create(2.f, CCPointZero)));
        retn->addChild(levelsList);
        //btn1
        retn->m_button1->m_BGSprite->setSpriteFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("geode.loader/baseEditor_Normal_DarkGray.png"));
        retn->m_button1->m_BGSprite->setContentSize(retn->m_button1->getContentSize());
        retn->m_button1->m_BGSprite->setPositionY(retn->m_button1->m_BGSprite->getPositionY() - 1);
        //btn2
        retn->m_button2->m_BGSprite->setSpriteFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("geode.loader/baseEditor_Normal_Gray.png"));
        retn->m_button2->m_BGSprite->setContentSize(retn->m_button2->getContentSize());
        retn->m_button2->m_BGSprite->setPositionY(retn->m_button2->m_BGSprite->getPositionY() - 1);
        return retn;
    }
    static bool TryIt(GJGameLevel* lvl) {
        pLevel = lvl;
		log::debug(__FUNCTION__);
        if (GetKeyState(KEY_G) & 0x8000 and GetKeyState(KEY_Control) & 0x8000) {
            pFLAlertLayer = create();
            pFLAlertLayer->show();
            return true;
        }
        return false;
    }
};

#include <Geode/modify/LevelInfoLayer.hpp>
class $modify(LevelInfoLayer) {
	void onPlay(CCObject* pSender) {
        if (GrabLevelPopup::TryIt(this->m_level)) return;
		LevelInfoLayer::onPlay(pSender);
	}
};
