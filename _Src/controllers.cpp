#include <Geode/Geode.hpp>
using namespace geode::prelude;

#include "Header1.hpp"
#include <Geode/modify/LevelSelectLayer.hpp>

void WriteProcMem(uintptr_t address, ByteVector const& data) {
    for (auto catgirl : Mod::get()->getPatches()) {
        if (catgirl and catgirl->getAddress() == address) {
            catgirl->updateBytes(data);
            return;
        }
    }
    Mod::get()->patch((void*)address, data);
}
//std::vector<uint8_t> { intToBytes(777)[0], intToBytes(777)[1], intToBytes(777)[2],intToBytes(777)[3] }
std::vector<unsigned char> intToBytes(int value) {
    std::vector<unsigned char> result;
    result.push_back(value & 0x000000ff);
    result.push_back((value & 0x0000ff00) >> 8);
    result.push_back((value & 0x00ff0000) >> 16);
    result.push_back((value & 0xff000000) >> 24);
    return result;
}

void UpdatePagesSetup() {

    std::string MainSection = fmt::format("UpdatePagesSetup");
    std::string IniPath = FilePathFromModFolder("_PagesSetupPatch.ini");

    CSimpleIni Ini;
    Ini.LoadFile(IniPath.c_str());

    //cmp_amount
    if (!(Ini.KeyExists(MainSection.c_str(), "cmp_amount")))
        Ini.SetLongValue(
            MainSection.c_str(),
            "cmp_amount",
            cmp_amount,
            "; level pages adding amount (u cant set more than 127!)"
        );
    else cmp_amount = Ini.GetLongValue(MainSection.c_str(), "cmp_amount");

    //start_from
    if (!(Ini.KeyExists(MainSection.c_str(), "start_from")))
        Ini.SetLongValue(
            MainSection.c_str(),
            "start_from",
            start_from,
            "; acts as the first value when adding the rest levels page"
        );
    else start_from = Ini.GetLongValue(MainSection.c_str(), "start_from");

    Ini.SaveFile(IniPath.c_str());

    if (not Mod::get()->getSettingValue<bool>("USE_PATCHES")) return;////////////////////////////////////////////////////////////////////////////////
#ifdef GEODE_IS_WINDOWS
    //we at LevelSelectLayer::init(LevelSelectLayer* pClass, int initLevel)
    //way: "Download the soundtrack" string, move up to sideart sprites

    /*  2.204 findings

    26812C(rva) - while amount
    opcodes: 83 FE 17 7C 9F 8B 5C 24 1C BE 03 00 00 00 8D 9B
    targ:__________^ 17(16)=23(10)
    asm: cmp esi, 0x17

    //2680BF(rva) - start from
    opcodes: be 1 0 0 0 89 44 24 18 8b d8 8d 9b
    targ:_______^ that is 1 we know
    asm: mov esi, 0x01

    */

    {
        //std::vector<unsigned char> toRewrite = intToBytes(cmp_amount);
        auto addr = geode::base::get() + 0x26812C;//patterns::find_pattern("83 FE ? 7C 9F 8B 5C 24 1C BE 03 00 00 00 8D 9B");
        WriteProcMem(addr, { 0x83, 0xFE, (uint8_t)cmp_amount });
    }

    {
        std::vector<unsigned char> toRewrite = intToBytes(start_from);
        auto addr = geode::base::get() + 0x2680BF;//patterns::find_pattern("be ???? 89 44 24 18 8b d8 8d 9b");
        //addr = gd::base + 0x2680BF; //2.204
        //
        // 
        // 
        // 
        // log((ReadProcMemAsStr(addr, 22)).c_str());
        WriteProcMem(addr, { 0xBE, toRewrite[0], toRewrite[1], toRewrite[2], toRewrite[3] });
    }
#else
    //0x3ebda2 (v2.2.1.3-armeabi-v7a) idk what the fuck is that
    //003EBDA0 BA F1 17 0F F0 D1 4F F0 03 0A 00 20 FF F7 34 FE
    //                               BL              _ZN7cocos2d7CCArray9addObjectEPNS_8CCObjectE ; cocos2d::CCArray::addObject(cocos2d::CCObject *)
    //  .text:003EBDA0                 CMP.W           R10, #0x17
    //.text:003EBDA4                 BNE             loc_3EBD88
    //.text : 003EBDA6               MOV.W           R10, #3
    //WriteProcMem(geode::base::get() + 0x3ebda2, { 0xba, 0xf1, (uint8_t)cmp_amount });
    //auto addr = geode::addresser::getVirtual(geode::modifier::AsStaticFunction_init<LevelSelectLayer, bool>::value);//<int>::func(::init)) + 0x2e2;
    //auto addr = geode::addresser::getVirtual(geode::modifier::Resolve<int>::func(&LevelSelectLayer::init)) + 0x2e2;
    //WriteProcMem(addr, { 0xba, 0xf1, (uint8_t)cmp_amount });
#endif
}

//huh
#include <Geode/modify/PlayLayer.hpp>
class $modify(PlayLayer) {
    static PlayLayer* create(GJGameLevel * level, bool useReplay, bool dontCreateObjects) {
        if (level->m_levelType == GJLevelType::Local) {
            auto levelDataPath = FilePathFromModFolder(fmt::format("levels/{}.txt", level->m_levelID.value()));
            if (checkExistence(levelDataPath)) {
                level->m_levelString = read_file(levelDataPath);
            }
            else level->m_levelString = LocalLevelManager::get()->getMainLevelString(level->m_levelID.value());
        };
        if (level->m_levelType == GJLevelType::Saved) {
            auto levelDataPath = FilePathFromModFolder(fmt::format("levels/{}.txt", level->m_levelID.value()));
            if (checkExistence(levelDataPath)) {
                level->m_levelString = read_file(levelDataPath);
            };
        };
        return PlayLayer::create(level, useReplay, dontCreateObjects);
    }
};

#include <Geode/modify/LoadingLayer.hpp>
class $modify(LoadingLayer) {
    void loadingFinished() {
        //create some inis
        LevelSelectLayer::create(0)->addChild(CCNode::create(), 999, 701123);
        for (int levelID = 0; levelID < 127; levelID++) {
            if (!checkExistence(FilePathFromModFolder(fmt::format("levels/{}.txt", levelID)))) {
                //save
                auto levelData = LocalLevelManager::get()->getMainLevelString(levelID);
                if (std::string(levelData) != "") {
                    std::string levelDataPath = FilePathFromModFolder(fmt::format("levels/{}.txt", levelID));
                    std::ofstream levelDataFile;
                    levelDataFile.open(levelDataPath);
                    levelDataFile.clear();
                    levelDataFile << levelData.data();
                    levelDataFile.close();
                }
            }
        }
        //call org
        LoadingLayer::loadingFinished();
    };
};

#include <Geode/loader/SettingEvent.hpp>
#include <Geode/modify/LevelSelectLayer.hpp>
class $modify(LevelSelectLayerExd, LevelSelectLayer) {
    bool init(int p0) {
        UpdatePagesSetup();
        auto rtn = LevelSelectLayer::init(p0);
        //if (this->getChildByTag(701123)) return rtn;
        //getNodes
        CCObject* obj = nullptr;
        CCSprite* pBackgroundSprite = nullptr;
        GJGroundLayer* pGJGroundLayer = nullptr;
        CCARRAY_FOREACH(this->getChildren(), obj)
        {
            auto node = dynamic_cast<CCNode*>(obj);
            if (node) {
                //pGJGroundLayer
                if (typeinfo_cast<GJGroundLayer*>(obj)) 
                    pGJGroundLayer = (GJGroundLayer*)(obj);
                //pBackgroundSprite
                auto sprite = typeinfo_cast<CCSprite*>(obj);
                if (sprite) {
                    if (framePath(sprite).find("GJ_gradientBG") != std::string::npos) pBackgroundSprite = sprite;
                }
            }
        }
        //setup nodes
        if (pBackgroundSprite) {
            auto BG_SPRITE_NAME = Mod::get()->getSettingValue<ghc::filesystem::path>("BG_SPRITE_NAME").filename();
            auto filename = BG_SPRITE_NAME.string();
            pBackgroundSprite->initWithFile(filename.c_str());
            pBackgroundSprite->setPosition(CCPointZero);
            pBackgroundSprite->setScale(CCDirector::get()->getScreenRight() / pBackgroundSprite->getContentSize().width);
            listenForSettingChanges("BG_SPRITE_NAME", +[](ghc::filesystem::path value) {
                value = ghc::filesystem::path(value).filename();
                if (CCDirector::get()->m_pRunningScene->getChildByTag(6481)) return;
                auto pop = geode::createQuickPopup(
                    "BG_SPRITE_NAME",
                    fmt::format("Will be used as:\n<cg>{}</c>\n\n<cr>btw if u dont see selected bg in back to this popup now so ur selection wrong</c>", value.string()),
                    "OK", nullptr,
                    nullptr
                );
                pop->setTag(6481);
                if (auto prev = CCSprite::create(value.string().c_str())) {
                    prev->setScale(CCDirector::get()->getScreenTop() / prev->getContentSize().height);
                    prev->setPosition(CCMenu::create()->getPosition());
                    prev->setPositionX(0.f);
                    prev->setAnchorPoint({ 0.f, 0.5f });
                    prev->setBlendFunc({ GL_SRC_ALPHA, GL_ONE });
                    prev->setColor(Mod::get()->getSettingValue<ccColor3B>("MAIN_COLOR"));
                    pop->addChild(prev, -1, 75289);
                };
            });
            //STRETCH_BG?
            if (Mod::get()->getSettingValue<bool>("STRETCH_BG")) {
                pBackgroundSprite->setScaleY(CCDirector::get()->getScreenTop() / pBackgroundSprite->getContentSize().height);
                pBackgroundSprite->setAnchorPoint(CCPointZero);
            }
            //BG_Y
            else pBackgroundSprite->setAnchorPoint({ 0.f, (float)Mod::get()->getSettingValue<double>("BG_Y") });
        }
        if (pGJGroundLayer) {
            pGJGroundLayer->setVisible(Mod::get()->getSettingValue<bool>("SHOW_GROUND"));
            int actualLoadedGroundID = GameManager::get()->m_loadedGroundID;
            pGJGroundLayer->init(
                Mod::get()->getSettingValue<int64_t>("GROUND_ID"),
                Mod::get()->getSettingValue<int64_t>("GROUND_LINE")
            );
            GameManager::get()->m_loadedGroundID = actualLoadedGroundID;
        }
        return rtn;
    };
    void updatePageIndicatorNode(ccColor3B _ccColor3B, int page) {
        auto node = this->getChildByID("page");
        if (!node) {
            auto newnode = CCLabelTTF::create("last page from colorForPage() sets to tag for this node", "arial", 1.f);
            newnode->setVisible(0);
            newnode->setID("page");
            this->addChild(newnode, 999);
            node = newnode;
        }
        node->setTag(page);
        ((CCNodeRGBA*)node)->setColor(_ccColor3B);
    }
    ccColor3B colorForPage(int page) {
        ccColor3B _ccColor3B = LevelSelectLayer::colorForPage(page);

        if (not Mod::get()->getSettingValue<bool>("OVERWRITE_COLOR")) {

            std::string IniPath = FilePathFromModFolder("_PageColors.ini");
            std::string MainSection = fmt::format("colorForPage");
            std::string MainVal = fmt::format("{}", page);

            CSimpleIni Ini;
            Ini.LoadFile(IniPath.c_str());

            //Color
            if (!(Ini.KeyExists(MainSection.c_str(), MainVal.c_str())))
                Ini.SetValue(
                    MainSection.c_str(),
                    MainVal.c_str(),
                    fmt::format("{},{},{}", _ccColor3B.r, _ccColor3B.g, _ccColor3B.b).c_str()
                );
            else {
                std::vector<std::string> rgb = explode(Ini.GetValue(MainSection.c_str(), MainVal.c_str()), ',');
                _ccColor3B.r = atoi(rgb[0].data());
                _ccColor3B.g = atoi(rgb[1].data());
                _ccColor3B.b = atoi(rgb[2].data());
            }

            Ini.SaveFile(IniPath.c_str());

        }
        else {
            _ccColor3B = Mod::get()->getSettingValue<ccColor3B>("MAIN_COLOR");
        }

        updatePageIndicatorNode(_ccColor3B, page);
        return _ccColor3B;
    }
    /**/
};

#if 1
#define crRetAAAsdp0(MainValaa, IniName) \
std::string MainSection = fmt::format("{}", p0); \
std::string MainVal = fmt::format("{}", MainValaa); \
std::string IniPath = IniName; \
CSimpleIni Ini; \
Ini.LoadFile(IniPath.c_str()); \
if (!(Ini.KeyExists(MainSection.c_str(), MainVal.c_str()))) \
{Ini.SetValue(MainSection.c_str(),MainVal.c_str(),crRet.c_str());} \
else {crRet = Ini.GetValue(MainSection.c_str(), MainVal.c_str());} \
 \
Ini.SaveFile(IniPath.c_str());
#endif

#include <Geode/modify/LevelTools.hpp>
class $modify(LevelTools) {
    static gd::string getAudioFileName(int p0) {
        std::string crRet = LevelTools::getAudioFileName(p0);
        crRetAAAsdp0("Filename", FilePathFromModFolder("_AudioTracks.ini"));
        return crRet;
    }
    static gd::string getAudioTitle(int p0) {
        gd::string crRet = LevelTools::getAudioTitle(p0);
        crRetAAAsdp0("Title", FilePathFromModFolder("_AudioTracks.ini"));
        return crRet;
    }
    static gd::string nameForArtist(int p0) {
        gd::string crRet = LevelTools::nameForArtist(p0);
        crRetAAAsdp0("name", FilePathFromModFolder("_Artists.ini"));
        return crRet;
    }
    static gd::string fbURLForArtist(int p0) {
        gd::string crRet = LevelTools::fbURLForArtist(p0);
        crRetAAAsdp0("fbURL", FilePathFromModFolder("_Artists.ini"));
        return crRet;
    }
    static gd::string ngURLForArtist(int p0) {
        gd::string crRet = LevelTools::ngURLForArtist(p0);
        crRetAAAsdp0("ngURL", FilePathFromModFolder("_Artists.ini"));
        return crRet;
    }
    static gd::string ytURLForArtist(int p0) {
        gd::string crRet = LevelTools::ytURLForArtist(p0);
        crRetAAAsdp0("ytURL", FilePathFromModFolder("_Artists.ini"));
        return crRet;
    }
    static GJGameLevel* getLevel(int p0, bool p1) {
        GJGameLevel* pGJGameLevel = processOutLevelByConfig(p0, LevelTools::getLevel(p0, p1));
        return pGJGameLevel;
    }
    static bool verifyLevelIntegrity(gd::string p0, int p1) {
        return 1;
    }
};
/**/

#include <Geode/modify/LevelInfoLayer.hpp>
class $modify(LevelInfoLayer) {
    void setupLevelInfo() {
        auto SL = Mod::get()->getSettingValue<bool>("SL");
        //orglevel
        if (SL) {
            this->m_level = processOutLevelByConfig(this->m_level->m_levelID.value(), this->m_level);
            //m_difficultySprite->updateFeatureStateFromLevel(this->m_level);
        }
        LevelInfoLayer::setupLevelInfo();
    }
    void levelDownloadFinished(GJGameLevel * p0) {
        auto SL = Mod::get()->getSettingValue<bool>("SL");
        //orglevel
        if (SL) {
            p0 = processOutLevelByConfig(p0->m_levelID.value(), p0);
            //m_difficultySprite->updateFeatureStateFromLevel(this->m_level);
        }
        LevelInfoLayer::levelDownloadFinished(p0);
        if (SL) {
            this->m_level = processOutLevelByConfig(this->m_level->m_levelID.value(), this->m_level);
            //m_difficultySprite->updateFeatureStateFromLevel(this->m_level);
        }
    }
};
#include <Geode/modify/LevelCell.hpp>
class $modify(LevelCellExt, LevelCell) {
    void loadFromLevel(GJGameLevel* p0) {
        auto SL = Mod::get()->getSettingValue<bool>("SL");
        //orglevel
        if (SL) {
            p0 = processOutLevelByConfig(p0->m_levelID.value(), p0);
            //m_difficultySprite->updateFeatureStateFromLevel(this->m_level);
        }
        LevelCell::loadFromLevel(p0);
    }
};