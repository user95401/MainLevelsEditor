#include <Geode/Geode.hpp>//Include the Geode headers.
#include <Geode/utils/web.hpp>//Include the Geode headers.
using namespace geode::prelude;//cocos2d and all Geode namespaces

#include "ModUtils.hpp"
#include "SimpleIni.h"

#include "Header1.hpp"

std::string truncate(std::string str, size_t width, bool show_ellipsis = true)
{
    if (str.length() > width)
        if (show_ellipsis)
            return str.substr(0, width) + "...";
        else
            return str.substr(0, width);
    return str;
}
int stoi(const std::string& str, int* p_value, std::size_t* pos = 0, int base = 10) {
    // wrapping std::stoi because it may throw an exception
    try {
        *p_value = std::stoi(str, pos, base);
        return 0;
    }
    catch (const std::invalid_argument& ia) {
        //std::cerr << "Invalid argument: " << ia.what() << std::endl;
        return -1;
    }
    catch (const std::out_of_range& oor) {
        //std::cerr << "Out of Range error: " << oor.what() << std::endl;
        return -2;
    }
    catch (const std::exception& e)
    {
        //std::cerr << "Undefined error: " << e.what() << std::endl;
        return -3;
    }
}
std::string framePath(CCNode* node) {
    if (auto textureProtocol = typeinfo_cast<CCTextureProtocol*>(node)) {
        if (auto texture = textureProtocol->getTexture()) {
            auto* cachedTextures = CCTextureCache::sharedTextureCache()->m_pTextures;
            for (auto [key, obj] : CCDictionaryExt<std::string, CCTexture2D*>(cachedTextures)) {
                if (obj == texture) {
                    return key.c_str();
                    break;
                }
            }

            if (auto spriteNode = typeinfo_cast<CCSprite*>(node)) {
                auto* cachedFrames = CCSpriteFrameCache::sharedSpriteFrameCache()->m_pSpriteFrames;
                const auto rect = spriteNode->getTextureRect();
                for (auto [key, frame] : CCDictionaryExt<std::string, CCSpriteFrame*>(cachedFrames)) {
                    if (frame->getTexture() == texture && frame->getRect() == rect) {
                        return key.c_str();
                        break;
                    }
                }
            }

        }
    }
    return "nah";
}

#include <Geode/cocos/support/zip_support/ZipUtils.h>
#include <regex>
class GrabLevelPopup : public FLAlertLayer {
public:
    inline static InputNode* pTextInput;
    inline static GJGameLevel* pLevel;
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
            auto pop = FLAlertLayer::create("Grab Level:", resultstr, "OK");
            //sync bg
            pop->m_mainLayer->getChildByID("background")->setContentSize(p0->m_mainLayer->getChildByID("background")->getContentSize());
            //btn1
            pop->m_button1->m_BGSprite->setSpriteFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("geode.loader/baseEditor_Normal_DarkGray.png"));
            pop->m_button1->m_BGSprite->setContentSize(pop->m_button1->getContentSize());
            pop->m_button1->m_BGSprite->setPositionY(pop->m_button1->m_BGSprite->getPositionY() - 1);
            //lets start the show
            pop->m_noElasticity = 1;
            pop->show();
            //grab level
            GJGameLevel* pGJGameLevel = pLevel;
            {
                std::string MainSection = std::format("Level Setup");
                std::string IniPath = FilePathFromModFolder(std::format("levels/setup/{}.ini", pTextInput->getString()));

                CSimpleIni Ini;
                Ini.LoadFile(IniPath.c_str());

                //m_sLevelName
                Ini.SetValue(
                    MainSection.c_str(),
                    "LevelName",
                    pGJGameLevel->m_levelName.c_str(),
                    "; Level Name"
                );

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
                    "difficulty",
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

                //m_stars
                auto stars = pGJGameLevel->m_starsRequested;
                Ini.SetLongValue(
                    MainSection.c_str(),
                    "stars",
                    stars,
                    "; Stars"
                );

                //m_audioTrack
                auto audioTrack = (pLevel->m_audioTrack <= 0) ? stoi(pTextInput->getString()) : pLevel->m_audioTrack;
                Ini.SetLongValue(
                    MainSection.c_str(),
                    "audioTrack",
                    audioTrack,
                    "; Audio Track ID"
                );

                //m_capacityString
                Ini.SetValue(
                    MainSection.c_str(),
                    "capacityString",
                    pGJGameLevel->m_capacityString.c_str(),
                    "; Capacity String"
                );

                //m_levelDesc
                Ini.SetValue(
                    MainSection.c_str(),
                    "levelDesc",
                    pGJGameLevel->m_levelDesc.c_str(),
                    "; m_levelDesc (useless)"
                );

                //m_creatorName
                Ini.SetValue(
                    MainSection.c_str(),
                    "creatorName",
                    pGJGameLevel->m_creatorName.c_str(),
                    "; m_creatorName (useless)"
                );

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
                std::string levelDataPath = FilePathFromModFolder(std::format("levels/{}.txt", pTextInput->getString()));
                std::ofstream levelData;
                levelData.open(levelDataPath);
                levelData.clear();
                levelData << level_data.data();
                levelData.close();
            };
            //_PagesSetupPatch
            {
                std::string MainSection = std::format("{}", "UpdatePagesSetup");
                std::string IniPath = FilePathFromModFolder("_PagesSetupPatch.ini");

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
            };
            //_AudioTracks
            if (pLevel->m_audioTrack <= 0) {
                auto newSongPath = FilePathFromModFolder(std::format("audio/{} ({}).mp3", pLevel->m_levelName.c_str(), pLevel->m_songID));

                std::ifstream src(pLevel->getAudioFileName(), std::ios::binary);
                std::ofstream dst(newSongPath, std::ios::binary);
                dst << src.rdbuf();

                if (!std::filesystem::exists(pLevel->getAudioFileName().data())) {
                    resultstr = resultstr.data() + std::string("\n<cr>song wasn't downloaded!</c>\n<co>download it first and retry...</c>");
                }

                std::string MainSection = std::format("{}", pTextInput->getString());
                std::string IniPath = FilePathFromModFolder("_AudioTracks.ini");

                CSimpleIniA Ini;
                Ini.LoadFile(IniPath.c_str());

                Ini.SetValue(MainSection.c_str(), "Title", pLevel->m_levelName.c_str());
                Ini.SetValue(MainSection.c_str(), "Filename", newSongPath.c_str());

                Ini.SaveFile(IniPath.c_str());
            };
            //set result string for text area
            dynamic_cast<TextArea*>(pop->getChildByIDRecursive("content-text-area"))->setString(resultstr);
        };
    };
    static FLAlertLayer* create() {
        CSimpleIni Ini;
        Ini.LoadFile(FilePathFromModFolder("_PagesSetupPatch.ini").data());
        int cmp_amount = Ini.GetLongValue("UpdatePagesSetup", "cmp_amount");
        auto cmp_amount_str = Ini.GetValue("UpdatePagesSetup", "cmp_amount");
        auto retn = FLAlertLayer::create(GrabLevelPopup::GrabLevelPopupProtocol::create(), "Grab Level:", gd::string(""), "Close", "Grab It!");
        //textInput
        pTextInput = InputNode::create(160.f, "main level id", "chatFont.fnt", "1234567890", 4);
        pTextInput->setPositionY(44.f);
        pTextInput->setString(cmp_amount_str);
        retn->m_buttonMenu->addChild(pTextInput);
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
    static void TryOpenIt(CCNode* inner) {
        if (GetKeyState(KEY_Insert) & 0x8000) {
            if (!inner) return;
            inner->removeChildByTag(1374);
            auto pFLAlertLayer = create();
            inner->addChild(pFLAlertLayer, 999, 1374);
        }
        if ((GetKeyState(KEY_Enter) & 0x8000) and inner->getChildByTag(1374))
            dynamic_cast<FLAlertLayer*>(inner->getChildByTag(1374))->onBtn2(CCNode::create());
    }
};

#include <Geode/modify/LevelInfoLayer.hpp>
class $modify(LevelInfoLayerExt, LevelInfoLayer) {
    void SomeSch(float) {
        if (!this) return;
        GrabLevelPopup::pLevel = this->m_level;
        GrabLevelPopup::TryOpenIt(this->getParent());
    }
    bool init(GJGameLevel* p0, bool p1) {
        this->schedule(schedule_selector(LevelInfoLayerExt::SomeSch), 0.01f);
		return LevelInfoLayer::init(p0, p1);
	}
};
#include <Geode/modify/RateStarsLayer.hpp>
class $modify(RateStarsLayerExt, RateStarsLayer) {
    inline static RateStarsLayer* m_pRateStarsLayer;
    void SomeSch(float) {
        if (!this) return;
        GrabLevelPopup::pLevel->m_starsRequested = m_starsRate;
    }
    static RateStarsLayer* create(int p0, bool p1, bool p2) {
        m_pRateStarsLayer = RateStarsLayer::create(p0, p1, p2);
        m_pRateStarsLayer->schedule(schedule_selector(RateStarsLayerExt::SomeSch), 0.01f);
        return m_pRateStarsLayer;
	}
};
#include <Geode/modify/ShareLevelLayer.hpp>
class $modify(ShareLevelLayerExt, ShareLevelLayer) {
    inline static GJGameLevel* m_level;
    inline static ShareLevelLayer* m_pShareLevelLayer;
    int starReq;
    void SomeSch(float) {
        if (!m_pShareLevelLayer) return;
        auto menu = m_pShareLevelLayer->m_buttonMenu;
        if (menu) {
            for (int i = 1; i <= 10; i++) {
                auto iNodeX3 = dynamic_cast<CCMenuItemSpriteExtra*>(menu->getChildByTag(i));
                if (iNodeX3) {
                    auto scale9spr = dynamic_cast<CCScale9Sprite*>(iNodeX3->getChildByTag(1)->getChildren()->objectAtIndex(0));
                    if (scale9spr) {
                        auto scale9sprbatchnode = dynamic_cast<CCSpriteBatchNode*>(scale9spr->getChildren()->objectAtIndex(0));
                        if (scale9sprbatchnode) {
                            auto sFramePath = framePath(scale9sprbatchnode);
                            if (strstr(sFramePath.c_str(), "GJ_button_01")) starReq = i;
                        }
                    };
                }
            }
        };
        m_level->m_starsRequested = starReq;
        GrabLevelPopup::pLevel = m_level;
        GrabLevelPopup::TryOpenIt(m_pShareLevelLayer);
    }
    static ShareLevelLayer* create(GJGameLevel* p0) {
        m_pShareLevelLayer = ShareLevelLayer::create(p0);
        m_level = p0;
        m_pShareLevelLayer->schedule(schedule_selector(ShareLevelLayerExt::SomeSch), 0.01f);
		return m_pShareLevelLayer;
	}
};

//DELETE LEVEL HELPER
static int g_currentPage;
static GJGameLevel* g_currentLevel;
static LevelSelectLayer* pLevelSelectLayer;
#define MEMBERBYOFFSET(type, class, offset) *reinterpret_cast<type*>(reinterpret_cast<uintptr_t>(class) + offset)
LevelPage* getPage(BoomScrollLayer* scroll, int a2) {
    bool v3; // zf
    cocos2d::CCArray* v4; // ecx
    signed int v5; // eax
    signed int v6; // edx
    v3 = MEMBERBYOFFSET(bool, scroll, 0x158) == 0;
    v4 = MEMBERBYOFFSET(CCArray*, scroll, 360);
    if (!v4) return LevelPage::create(g_currentLevel);
    if (v3) return (LevelPage*)v4->objectAtIndex(a2);
    v5 = v4->count();
    v6 = a2;
    if (a2 > 0) return (LevelPage*)v4->objectAtIndex(a2 % v5);
    if (a2 < 0) {
        do v6 += v5;
        while (v6 < 0);
    }
    return (LevelPage*)v4->objectAtIndex(v6);
}
#include <Geode/modify/LevelSelectLayer.hpp>
class $modify(LevelSelectLayerExt, LevelSelectLayer) {
    CCNode* getDelPop() {
        return pLevelSelectLayer->getChildByTag(5629);
    }
    void onDelete(CCObject*) {
        //level setup
        std::remove(FilePathFromModFolder(std::format("levels/setup/{}.ini", g_currentPage)).c_str());
        //data store
        std::remove(FilePathFromModFolder(std::format("levels/{}.txt", g_currentPage)).c_str());
        //_PagesSetupPatch
        {
            std::string MainSection = std::format("{}", "UpdatePagesSetup");
            std::string IniPath = FilePathFromModFolder("_PagesSetupPatch.ini");
            CSimpleIni Ini;
            Ini.LoadFile(IniPath.c_str());
            Ini.SetLongValue(
                MainSection.c_str(),
                "cmp_amount",
                (Ini.GetLongValue(MainSection.c_str(), "cmp_amount")) - 1,
                "; level pages adding amount (u cant set more than 127!)"
            );
            Ini.SaveFile(IniPath.c_str());
        };
        //_AudioTracks
        {
            std::string MainSection = std::format("{}", g_currentLevel->m_audioTrack);
            std::string IniPath = FilePathFromModFolder("_AudioTracks.ini");

            CSimpleIniA Ini;
            Ini.LoadFile(IniPath.c_str());

            Ini.Delete(MainSection.c_str(), NULL);

            Ini.SaveFile(IniPath.c_str());
        };
        //asshell
        {
            //level setup
            for (size_t levelID = 0; levelID < 127; levelID++) {
                if (levelID > g_currentPage and levelID < 127) {
                    std::rename(
                        FilePathFromModFolder(std::format("levels/setup/{}.ini", levelID)).c_str(),
                        FilePathFromModFolder(std::format("levels/setup/{}.ini", levelID - 1)).c_str()
                    );
                };
            }
            //level data
            for (size_t levelID = 0; levelID < 127; levelID++) {
                if (levelID > g_currentPage) {
                    if (
                        !std::filesystem::exists(FilePathFromModFolder(std::format("levels/{}.txt", levelID)))
                        and std::filesystem::exists(std::format("Resources/levels/{}.txt", levelID))
                        )
                        std::filesystem::copy_file(
                            std::format("Resources/levels/{}.txt", levelID).c_str(),
                            FilePathFromModFolder(std::format("levels/{}.txt", levelID - 1)).c_str()
                        );
                    else
                        std::rename(
                            FilePathFromModFolder(std::format("levels/{}.txt", levelID)).c_str(),
                            FilePathFromModFolder(std::format("levels/{}.txt", levelID - 1)).c_str()
                        );
                }
            }
        }
        //a
        Notification::create(std::format("{} deleted...", g_currentLevel->m_levelName.data()).data(), NotificationIcon::Warning)->show();
        if (pLevelSelectLayer->getChildByTag(5629)) pLevelSelectLayer->getChildByTag(5629)->removeMeAndCleanup();
        if (pLevelSelectLayer) {
            pLevelSelectLayer->removeAllChildren();
            pLevelSelectLayer->init(fabs(g_currentPage - 1));
        }
    }
    void openDeletePopup() {
        if (!pLevelSelectLayer) return;
        if (getDelPop()) getDelPop()->removeMeAndCleanup();
        FLAlertLayer* deletePop = FLAlertLayer::create(nullptr, std::format("Delete {}'th Level", g_currentPage).c_str(), std::format("<co>Delete</c> level data, setup;\n try <cr>adaptate</c> <co>all next levels</c>;\nand <cy>decrease</c> level pages count?"), "Nah", "Delete");
        //btn1
        if (deletePop->m_button1 and deletePop->m_button1->m_BGSprite) {
            auto SpriteFrame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("geode.loader/baseEditor_Normal_DarkGray.png");
            if (SpriteFrame) {
                deletePop->m_button1->m_BGSprite->setSpriteFrame(SpriteFrame);
                deletePop->m_button1->m_BGSprite->setContentSize(deletePop->m_button1->getContentSize());
                deletePop->m_button1->m_BGSprite->setPositionY(deletePop->m_button1->m_BGSprite->getPositionY() - 1);
            };
        };
        //btn2
        if (deletePop->m_button2 and deletePop->m_button2->m_BGSprite) {
            auto SpriteFrame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("geode.loader/baseEditor_Normal_Gray.png");
            if (SpriteFrame) {
                deletePop->m_button2->m_BGSprite->setSpriteFrame(SpriteFrame);
                deletePop->m_button2->m_BGSprite->setContentSize(deletePop->m_button2->getContentSize());
                deletePop->m_button2->m_BGSprite->setPositionY(deletePop->m_button2->m_BGSprite->getPositionY() - 1);
            };
            typeinfo_cast<CCMenuItemSpriteExtra*>(deletePop->m_button2->getParent())->setTarget(CCNode::create(), menu_selector(LevelSelectLayerExt::onDelete));
        };
        //Next
        if (deletePop->m_buttonMenu) {
            auto Next = CCMenuItemSpriteExtra::create(
                CCSprite::createWithSpriteFrameName("edit_areaModeBtn03_001.png"),
                pLevelSelectLayer,
                menu_selector(LevelSelectLayer::onNext)
            );
            Next->setAnchorPoint({ 0.f , 0.5f });
            Next->setPositionX(92.f);
            deletePop->m_buttonMenu->addChild(Next);
        };
        //Prev
        if (deletePop->m_buttonMenu) {
            auto Prev = CCMenuItemSpriteExtra::create(
                CCSprite::createWithSpriteFrameName("edit_areaModeBtn03_001.png"),
                pLevelSelectLayer,
                menu_selector(LevelSelectLayer::onPrev)
            );
            dynamic_cast<CCSprite*>(Prev->getNormalImage())->setFlipX(1);
            Prev->setAnchorPoint({ 1.f , 0.5f });
            Prev->setPositionX(-92.f);
            deletePop->m_buttonMenu->addChild(Prev);
        }
        //showup
        deletePop->m_mainLayer->setPositionY(-74.f);
        pLevelSelectLayer->addChild(deletePop, 999, 5629);
        return;
    };
    void updateDeletePopTitle() {
        if (!getDelPop()) return;
        dynamic_cast<CCLabelBMFont*>(getDelPop()->getChildByIDRecursive("title"))->setString(std::format("Delete {}'th Level", g_currentPage).c_str());
    }
    void SomeSch(float asd) {
        if (!pLevelSelectLayer) return;
        BoomScrollLayer* ohfuck = MEMBERBYOFFSET(BoomScrollLayer*, pLevelSelectLayer, 336);
        int newPage = getPage(ohfuck, MEMBERBYOFFSET(int, ohfuck, 460))->m_level->m_levelID.value();
        if (newPage != g_currentPage) {
            g_currentPage = newPage;
            updateDeletePopTitle();
        }
        g_currentLevel = getPage(ohfuck, MEMBERBYOFFSET(int, ohfuck, 460))->m_level;
        //log::debug("{}", g_currentPage);
        if ((GetKeyState(KEY_Delete) & 0x8000)) openDeletePopup();
        if ((GetKeyState(KEY_Enter) & 0x8000) and getDelPop()) onDelete(CCNode::create());
    }
    void SomeSch2(float asd) {
        if (!pLevelSelectLayer) return;
        if ((GetKeyState(KEY_Right) & 0x8000) and getDelPop()) onNext(CCNode::create());
        if ((GetKeyState(KEY_Left) & 0x8000) and getDelPop()) onPrev(CCNode::create());
    }
    static LevelSelectLayer* create(int p0) {
        pLevelSelectLayer = LevelSelectLayer::create(p0);
        pLevelSelectLayer->schedule(schedule_selector(LevelSelectLayerExt::SomeSch), 0.01f);
        pLevelSelectLayer->schedule(schedule_selector(LevelSelectLayerExt::SomeSch2), 0.1f);
        return pLevelSelectLayer;
    };
};