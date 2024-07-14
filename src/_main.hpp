#pragma once
#include <Geode/Geode.hpp>
using namespace geode::prelude;

#include <regex>

//lol
#define SETTING(type, key_name) Mod::get()->getSettingValue<type>(key_name)

inline auto levels_path = Mod::get()->getConfigDir() / "levels";
inline auto levels_meta_path = levels_path / "_meta";
inline auto songs_path = Mod::get()->getConfigDir() / "songs";
inline auto audios_meta_path = songs_path / "_audios";
inline auto artists_meta_path = songs_path / "_artists";
namespace fs {
    using namespace std::filesystem;
    inline auto last_err = std::string();
    inline std::error_code last_err_code;
    template <typename T> inline auto rtnWithErrLog(T rtn, std::string log) { log::error("{}", log); return rtn; }
    inline auto exists(path path, std::string& err = last_err) {
        return cocos::fileExistsInSearchPaths(path.string().c_str());
    }
    inline auto read(path path, std::string& err = last_err) {
        err = fmt::format("{}(path \"{}\") err: path dnt exists", __func__, path);
        if (not fs::exists(path)) return rtnWithErrLog(err, err); err = "";

        unsigned long file_size = 0;
        auto buffer = CCFileUtils::sharedFileUtils()->getFileData(path.string().c_str(), "rb", &file_size);

        std::string data = "read failed...";
        if (buffer && file_size != 0) data = std::string(reinterpret_cast<char*>(buffer), file_size);
        else return rtnWithErrLog(data, err);

        return data;
    }
    inline auto rename(path old_path, path new_path, std::string& err = last_err) {
        err = fmt::format("{}(old_path \"{}\", old_path \"{}\") err: old path dnt exists", __func__, old_path, new_path);
        if (not fs::exists(old_path)) return rtnWithErrLog(false, err);
        err = fmt::format("{}(old_path \"{}\", old_path \"{}\") err: new path exists", __func__, old_path, new_path);
        if (fs::exists(new_path)) return rtnWithErrLog(false, err);
        err = "";
        std::filesystem::rename(old_path, new_path, last_err_code);
        log::debug("{}(old_path \"{}\", old_path \"{}\"): last_err_code={}", __func__, old_path, new_path, last_err_code);
        return true;
    }
}

#define public_cast(value, member) [](auto* v) { \
	class FriendClass__; \
	using T = std::remove_pointer<decltype(v)>::type; \
	class FriendeeClass__: public T { \
	protected: \
		friend FriendClass__; \
	}; \
	class FriendClass__ { \
	public: \
		auto& get(FriendeeClass__* v) { return v->member; } \
	} c; \
	return c.get(reinterpret_cast<FriendeeClass__*>(v)); \
}(value)

namespace geode::cocos {
    inline std::string frameName(CCNode* node) {
        if (node == nullptr) return "NIL_NODE";
        if (auto textureProtocol = dynamic_cast<CCTextureProtocol*>(node)) {
            if (auto texture = textureProtocol->getTexture()) {
                if (auto spriteNode = dynamic_cast<CCSprite*>(node)) {
                    auto* cachedFrames = CCSpriteFrameCache::sharedSpriteFrameCache()->m_pSpriteFrames;
                    const auto rect = spriteNode->getTextureRect();
                    for (auto [key, frame] : CCDictionaryExt<std::string, CCSpriteFrame*>(cachedFrames)) {
                        if (frame->getTexture() == texture && frame->getRect() == rect) {
                            return key.c_str();
                        }
                    }
                }
                auto* cachedTextures = CCTextureCache::sharedTextureCache()->m_pTextures;
                for (auto [key, obj] : CCDictionaryExt<std::string, CCTexture2D*>(cachedTextures)) {
                    if (obj == texture) {
                        return key.c_str();
                    }
                }
            }
        }
        auto btnSpriteTry = frameName(getChild(node, 0));
        if (
            btnSpriteTry != "NIL_NODE"
            and btnSpriteTry != "CANT_GET_FRAME_NAME"
            ) return btnSpriteTry;
        return "CANT_GET_FRAME_NAME";
    }
};

namespace mle_leveltools {
    inline matjson::Value levelFromJson(matjson::Value value, GJGameLevel* levelToRewrite = nullptr) {
        GJGameLevel* level = levelToRewrite ? levelToRewrite : LevelTools::getLevel(1, 0);
        level->m_levelName = value.try_get<std::string>("m_levelName").value_or(level->m_levelName.data());
        level->m_levelDesc = value.try_get<std::string>("m_levelDesc").value_or(level->m_levelDesc.data());
        level->m_creatorName = value.try_get<std::string>("m_creatorName").value_or(level->m_creatorName.data());
        level->m_difficulty = (GJDifficulty)value.try_get<int>("m_difficulty").value_or((int)level->m_difficulty);
        level->m_stars = value.try_get<int>("m_stars").value_or(level->m_stars.value());
        level->m_audioTrack = value.try_get<int>("m_audioTrack").value_or(level->m_audioTrack);
        level->m_songID = value.try_get<int>("m_songID").value_or(level->m_songID);
        level->m_levelVersion = value.try_get<int>("m_levelVersion").value_or(level->m_levelVersion);
        level->m_gameVersion = value.try_get<int>("m_gameVersion").value_or(level->m_gameVersion);
        level->m_levelType = (GJLevelType)value.try_get<int>("m_levelType").value_or((int)level->m_levelType);
        level->m_capacityString = value.try_get<std::string>("m_capacityString").value_or(level->m_capacityString.data());
        level->m_songIDs = value.try_get<std::string>("m_songIDs").value_or(level->m_songIDs.data());
        level->m_sfxIDs = value.try_get<std::string>("m_sfxIDs").value_or(level->m_sfxIDs.data());
        return value;
    }
    inline auto jsonFromLevel(GJGameLevel* level) {
        auto value = matjson::Value();
        value.try_set("________MAIN_STUFF________", " vvv vvv ");
        value.try_set("m_levelName", std::string(level->m_levelName.data()));
        value.try_set("m_audioTrack", level->m_audioTrack);
        value.try_set("m_difficulty", (int)level->m_difficulty);
        value.try_set("m_stars", level->m_stars.value());
        value.try_set("________SOME_SHIT________", " vvv vvv ");
        value.try_set("m_levelDesc", std::string(level->m_levelDesc.data()));
        value.try_set("m_creatorName", std::string(level->m_creatorName.data()));;
        value.try_set("m_songID", level->m_songID);
        value.try_set("m_levelVersion", level->m_levelVersion);
        value.try_set("m_gameVersion", level->m_gameVersion);
        value.try_set("m_levelType", (int)level->m_levelType);
        value.try_set("m_capacityString", std::string(level->m_capacityString.data()));
        value.try_set("m_songIDs", std::string(level->m_songIDs.data()));
        value.try_set("m_sfxIDs", std::string(level->m_sfxIDs.data()));
        return value;
    }
    inline void updateLevelByJson(GJGameLevel* level) {
        auto level_meta_file = levels_meta_path / fmt::format("{}.json", level->m_levelID.value());
        auto file_content = fs::read(level_meta_file);
        //json val
        auto value = matjson::parse("{}");
        //file parse
        auto error = std::string();
        auto parse = matjson::parse(file_content, error);
        if (parse.has_value()) value = parse.value();
        //setup level
        levelFromJson(parse, level);
        //setup json
        value = jsonFromLevel(level);
        //save json
        std::ofstream(level_meta_file) << value.dump(matjson::TAB_INDENTATION);
    }
    inline auto replaceSecretCoinsByUserOnesInLevel(GJGameLevel* level) {
#ifdef DEBUG
        GEODE_WINDOWS(
            CCMessageBox(
                "U USE DEBUG VER!\nCOINS WILL NOT BE REPLACED!",
                Mod::get()->getName().c_str()
            )
        );
        return level;
#endif // DEBUG
        //user coin id 1329, 142 secret coin
        gd::string decompressed = ZipUtils::decompressString(
            level->m_levelString, true, 0
        );
        decompressed = std::regex_replace(std::string(decompressed.data()), std::regex(",142,"), ",1329,").data();
        level->m_levelString = ZipUtils::compressString(decompressed, true, 0);
        return level;
    }
    inline auto replaceUserCoinsBySecretOnesInLevel(GJGameLevel* level) {
#ifdef DEBUG
        GEODE_WINDOWS(
            CCMessageBox(
                "U USE DEBUG VER!\nCOINS WILL NOT BE REPLACED!",
                Mod::get()->getName().c_str()
            )
        );
        return level;
#endif // DEBUG
        //user coin id 1329, 142 secret coin
        gd::string decompressed = ZipUtils::decompressString(
            level->m_levelString, true, 0
        );
        decompressed = std::regex_replace(std::string(decompressed.data()), std::regex(",1329,"), ",142,").data();
        level->m_levelString = ZipUtils::compressString(decompressed, true, 0);
        return level;
    }
    inline auto updateLevelDataAndMetaFiles(gd::string str, GJGameLevel* level) {
        level->m_levelString = str;
        level = replaceUserCoinsBySecretOnesInLevel(level);
        return;
        auto level_path_to_save = (levels_path / fmt::format("{}.txt", level->m_levelID.value()));
        std::ofstream(level_path_to_save) << std::string(level->m_levelString.data());
        auto level_meta_file = levels_meta_path / fmt::format("{}.json", level->m_levelID.value());
        std::ofstream(level_meta_file) << jsonFromLevel(level).dump(matjson::TAB_INDENTATION);
    }
};

namespace mle_audiotools {
    class Audio {
    public:
        std::string m_fileName;
        std::string m_title;
        std::string m_url;
        int m_artistID;
        int m_audioID;
        matjson::Value m_json = matjson::Value();
        inline static bool CallDefaults = false;
        static void updateFromFile(Audio* pAudio) {
            //update by existing
            auto song_meta_file = audios_meta_path / fmt::format("{}.json", pAudio->m_audioID);
            if (!fs::exists(song_meta_file)) return;
            auto file_content = fs::read(song_meta_file);
            //file parse
            auto value = matjson::parse("{}");
            auto error = std::string();
            auto parse = matjson::parse(file_content, error);
            if (parse.has_value()) value = parse.value();
            //set from json json
            pAudio->m_fileName = value.try_get<std::string>("m_fileName").value_or(pAudio->m_fileName.data()).data();
            pAudio->m_title = value.try_get<std::string>("m_title").value_or(pAudio->m_title.data()).data();
            pAudio->m_url = value.try_get<std::string>("m_url").value_or(pAudio->m_url.data()).data();
            pAudio->m_artistID = value.try_get<int>("m_artistID").value_or(pAudio->m_artistID);
        }
        auto resetJson() {
            this->m_json.try_set("m_fileName", this->m_fileName);
            this->m_json.try_set("m_title", this->m_title);
            this->m_json.try_set("m_url", this->m_url);
            this->m_json.try_set("m_artistID", this->m_artistID);
            return this->m_json;
        }
        void openInfoLayer();
        Audio(int id = 1, bool noDefaults = true) {
            //default
            this->m_audioID = id;
            if (not noDefaults) {
                CallDefaults = 1;
                this->m_fileName = LevelTools::getAudioFileName(id).data();
                this->m_title = LevelTools::getAudioTitle(id).data();
                this->m_url = LevelTools::urlForAudio(id).data();
                this->m_artistID = LevelTools::artistForAudio(id);
                CallDefaults = 0;
            }
            //set vars from
            Audio::updateFromFile(this);
            //json
            resetJson();
            //save json
            if (not noDefaults and this->m_title == "Unknown") return;
            auto song_meta_file = audios_meta_path / fmt::format("{}.json", id);
            std::ofstream(song_meta_file) << this->m_json.dump(matjson::TAB_INDENTATION);
        };
    };
    class Artist {
    public:
        std::string m_ytURL;
        std::string m_ngURL;
        std::string m_name;
        int m_artistID;
        matjson::Value m_json;
        inline static bool CallDefaults = false;
        inline static void updateFromFile(Artist* pAudio) {
            //update by existing
            auto song_meta_file = artists_meta_path / fmt::format("{}.json", pAudio->m_artistID);
            if (!fs::exists(song_meta_file)) return;
            auto file_content = fs::read(song_meta_file);
            //file parse
            auto value = matjson::parse("{}");
            auto error = std::string();
            auto parse = matjson::parse(file_content, error);
            if (parse.has_value()) value = parse.value();
            //set from json json
            pAudio->m_ytURL = value.try_get<std::string>("m_ytURL").value_or(pAudio->m_ytURL.data()).data();
            pAudio->m_ngURL = value.try_get<std::string>("m_ngURL").value_or(pAudio->m_ngURL.data()).data();
            pAudio->m_name = value.try_get<std::string>("m_name").value_or(pAudio->m_name.data()).data();
            pAudio->m_artistID = value.try_get<int>("m_artistID").value_or(pAudio->m_artistID);
        }
        auto resetJson() {
            this->m_json.try_set("m_ytURL", this->m_ytURL);
            this->m_json.try_set("m_ngURL", this->m_ngURL);
            this->m_json.try_set("m_name", this->m_name);
            return this->m_json;
        };
        Artist(int id = 1, bool noDefaults = true) {
            //default
            this->m_artistID = id;
            if (not noDefaults) {
                CallDefaults = 1;
                this->m_ytURL = LevelTools::ytURLForArtist(id).data();
                this->m_ngURL = LevelTools::ngURLForArtist(id).data();
                this->m_name = LevelTools::nameForArtist(id).data();
                CallDefaults = 0;
            }
            //set vars from
            Artist::updateFromFile(this);
            //json
            resetJson();
            //save json
            if (not noDefaults and this->m_name == " ") return;
            auto song_meta_file = artists_meta_path / fmt::format("{}.json", id);
            std::ofstream(song_meta_file) << this->m_json.dump(matjson::TAB_INDENTATION);
        };
    }; 
    inline void Audio::openInfoLayer() {
        auto artist = Artist(this->m_artistID);
        auto sobj = SongInfoObject::create(this->m_audioID, this->m_title, artist.m_name, artist.m_artistID, 1337.f, artist.m_ngURL, artist.m_ytURL, this->m_url, 0, "", 0, 0, 0);
        auto asd = CustomSongWidget::create(sobj, nullptr, 0, 0, 0, 1, 0, 0, 0);
        asd->onMore(asd);
    };
}

namespace mle_ui {
    inline auto deleteButtonSprite() {
        auto hi = geode::AccountButtonSprite::create(
            CCSprite::createWithSpriteFrameName("edit_delBtn_001.png"),
            AccountBaseColor::Gray
        );
        hi->setScale(0.8f);
        typeinfo_cast<CCSprite*>(hi->getChildren()->objectAtIndex(0))->setScale(1.1f);
        return hi;
    }
    inline auto settingsButtonSprite() {
        auto hi = geode::AccountButtonSprite::create(
            CCSprite::createWithSpriteFrameName("d_cogwheel_04_001.png"),
            AccountBaseColor::Gray
        );
        hi->setScale(0.8f);
        typeinfo_cast<CCSprite*>(hi->getChildren()->objectAtIndex(0))->setScale(1.1f);
        typeinfo_cast<CCSprite*>(hi->getChildren()->objectAtIndex(0))->setColor({ 20, 20, 20 });
        hi->addChild(typeinfo_cast<CCSprite*>(hi->getChildren()->objectAtIndex(0)));//make it darker
        return hi;
    }
    class ArtistsLayer : public GJDropDownLayer, CCScrollLayerExtDelegate {
    public:
        inline static CCPoint lastScrollPos = CCPoint(999.f, 0.f);
        ScrollLayer* m_scrollLayer = nullptr;
        struct FileNumbericSort {
            bool operator()(const fs::path& a, const fs::path& b) const {
                auto stra = std::regex_replace(a.filename().string(), std::regex(a.filename().extension().string()), "");
                auto strb = std::regex_replace(b.filename().string(), std::regex(b.filename().extension().string()), "");
                auto inta = utils::numFromString<int>(stra).value_or(0);
                auto intb = utils::numFromString<int>(strb).value_or(0);
                return inta <= intb;
            }
        };
        auto addArtistsCell(int id, bool& altBg, CCContentLayer* contentLayer) {
            auto width = contentLayer->getContentWidth();
            //hr
            if (altBg) contentLayer->addChild(CCLayerColor::create(
                ccColor4B(161 - 20, 88 - 20, 44 - 20, 255),
                width, 1.00f
            ));
            //item a 
            auto entry = CCLayerColor::create(
                altBg ? ccColor4B(161, 88, 44, 255) : ccColor4B(194, 114, 62, 255),
                width, 60.000f
            );
            entry->setLayout(AnchorLayout::create());
            //content
            auto artist = mle_audiotools::Artist(id);
            auto btnAspectWidth = width - 100.0f;
            auto nameStr = fmt::format("{}", artist.m_name);
            if (auto nameLabel = CCLabelBMFont::create(nameStr.c_str(), "bigFont.fnt")) {
                nameLabel->setScale(0.7f);
                nameLabel->setAnchorPoint(CCPoint(0.f, 0.5f));
                //fitUpScale
                if (nameLabel->getContentWidth() > btnAspectWidth) {
                    auto fitUpScale = btnAspectWidth / nameLabel->getContentWidth();
                    if (fitUpScale < nameLabel->getScale()) nameLabel->setScale(fitUpScale);
                }
                //point
                nameLabel->setLayoutOptions(
                    AnchorLayoutOptions::create()
                    ->setAnchor(Anchor::TopLeft)
                    ->setOffset(CCPoint(10.f, -20.f))
                );
                //add
                entry->addChild(nameLabel);
                entry->updateLayout();
            }
            auto idStr = fmt::format("ID: {}", artist.m_artistID);
            if (auto artistLabel = CCLabelBMFont::create(idStr.c_str(), "goldFont.fnt")) {
                artistLabel->setScale(0.7f);
                artistLabel->setAnchorPoint(CCPoint(0.f, 0.5f));
                //fitUpScale
                if (artistLabel->getContentWidth() > btnAspectWidth) {
                    auto fitUpScale = btnAspectWidth / artistLabel->getContentWidth();
                    if (fitUpScale < artistLabel->getScale()) artistLabel->setScale(fitUpScale);
                }
                //point
                artistLabel->setLayoutOptions(
                    AnchorLayoutOptions::create()
                    ->setAnchor(Anchor::BottomLeft)
                    ->setOffset(CCPoint(10.f, 19.f))
                );
                //add
                entry->addChild(artistLabel);
                entry->updateLayout();
            }
            if (auto viewBtnSprite = ButtonSprite::create("View", 1.f)) {
                viewBtnSprite->m_BGSprite->setContentSize(CCSize(62.65f, 30.f));
                viewBtnSprite->m_label->setFntFile("bigFont.fnt");
                viewBtnSprite->m_label->setScale(0.6f);
                auto menu = CCMenu::createWithItem(
                    CCMenuItemSpriteExtra::create(viewBtnSprite, this, menu_selector(ArtistsLayer::onView))
                );
                cocos::getChildOfType<CCMenuItemSpriteExtra>(menu, 0)->setTag(id);
                menu->setLayoutOptions(
                    AnchorLayoutOptions::create()
                    ->setAnchor(Anchor::Right)
                    ->setOffset(CCPoint(-42.f, 0.f))
                );
                entry->addChild(menu);
                entry->updateLayout();
            }
            //add
            contentLayer->addChild(entry, 0, id);
            contentLayer->setContentHeight(
                contentLayer->getContentHeight() + 1.1f + entry->getContentHeight()
            );
            //hr
            if (altBg) contentLayer->addChild(CCLayerColor::create(
                ccColor4B(161 - 20, 88 - 20, 44 - 20, 255),
                width, 1.00f
            ));
            //sw alt
            altBg = !altBg;
            return entry;
        }
        static auto create() {
            auto rtn = new ArtistsLayer;
            rtn->init("Artists Dir");
            //ScrollLayer::create
            rtn->m_scrollLayer = ScrollLayer::create(rtn->m_listLayer->getContentSize());
            rtn->m_scrollLayer->setID("scrollLayer"_spr);
            rtn->m_scrollLayer->m_delegate = rtn;
            rtn->m_listLayer->addChild(rtn->m_scrollLayer);
            //fill contentLayer
            auto& contentLayer = rtn->m_scrollLayer->m_contentLayer;
            contentLayer->setContentHeight(0.f);
            contentLayer->setLayout(
                ColumnLayout::create()
                ->setGap(0)
                ->setAxisReverse(true)
                ->setAxisAlignment(AxisAlignment::End)
            );
            auto altBg = true;
            std::set<fs::path, FileNumbericSort> aFileNumbericSortSet;
            for (auto& entry : fs::directory_iterator(artists_meta_path, fs::last_err_code))
                aFileNumbericSortSet.insert(entry.path());
            for (auto& filepath : aFileNumbericSortSet) {
                if (filepath.filename().extension() == ".json") {
                    auto name = std::regex_replace(
                        filepath.filename().string(),
                        std::regex(filepath.filename().extension().string()),
                        ""
                    );
                    auto id = utils::numFromString<int>(name).value_or(0);
                    rtn->addArtistsCell(id, altBg, contentLayer);
                };
            }
            //add new btn
            if (auto newArtistBtnCell = rtn->addArtistsCell(0, altBg, contentLayer)) {
                newArtistBtnCell->removeAllChildren();
                auto item = CCMenuItemSpriteExtra::create(
                    ButtonSprite::create("Add Artist"),
                    rtn, menu_selector(ArtistsLayer::onNew)
                );
                auto menu = CCMenu::createWithItem(item);
                menu->setPosition(newArtistBtnCell->getContentSize() / 2);
                newArtistBtnCell->addChild(menu);
            }
            //update after adding
            contentLayer->setContentHeight(
                contentLayer->getContentHeight() > contentLayer->getParent()->getContentHeight() ?
                contentLayer->getContentHeight() :
                contentLayer->getParent()->getContentHeight()
            );
            contentLayer->updateLayout();
            rtn->m_scrollLayer->scrollToTop();
            if (lastScrollPos.x < 999.f) contentLayer->setPositionY(lastScrollPos.y);
            return rtn;
        }
        virtual void scrollViewDidEndMoving(CCScrollLayerExt* p0) {
            if (p0->m_contentLayer->getPositionY() < 0)
                lastScrollPos = p0->m_contentLayer->getPosition();
        }
        void onView(CCObject* obj);
        void onNew(CCObject* obj) {
            auto last_id = this->m_scrollLayer->m_contentLayer->getChildren()->count();
            for (last_id; last_id >= 0; last_id--) {
                if (this->m_scrollLayer->m_contentLayer->getChildByTag(last_id)) break;
            }
            auto artist = mle_audiotools::Artist(last_id + 1);
            this->hideLayer(1);
            auto newArtistsLayer = create();
            newArtistsLayer->setZOrder(this->getZOrder());
            newArtistsLayer->showLayer(1);
            newArtistsLayer->m_scrollLayer->m_contentLayer->setPositionY(0.f);
        }
    };
    class LevelConfigPopup : public FLAlertLayer, FLAlertLayerProtocol, TextInputDelegate {
    public:
        GJGameLevel* m_level = nullptr;
        std::function<void()> m_onSave = []() {};
        static auto create(GJGameLevel* for_level = LevelTools::getLevel(1, 0)) {
            auto rtn = new LevelConfigPopup;
            rtn->init(
                rtn,
                "Level Meta Config",
                "\n \n \n \n \n \n \n \n ",
                "Close", "Save", 352.000f, 0, 290.000f, 1.f
            );
            rtn->setID("LevelConfigPopup");
            rtn->m_level = for_level;
            rtn->customSetup();
            return rtn;
        }
        void onPlayMusic(CCObject* toggleItemObj) {
            auto m_audioTrack = typeinfo_cast<CCTextInputNode*>(this->getChildByIDRecursive("m_audioTrack")); if (not m_audioTrack) return;
            int audioTrack = utils::numFromString<int>(m_audioTrack->getString().data()).value_or(m_level->m_audioTrack);
            auto toggleItem = typeinfo_cast<CCMenuItemToggler*>(toggleItemObj); if (not toggleItem) return;
            toggleItem->isOn() ?
                GameManager::get()->fadeInMenuMusic() :
                GameManager::get()->fadeInMusic(mle_audiotools::Audio(audioTrack).m_fileName);
        }
        void onBrowseSongs(CCObject* asd) {
            this->setZOrder(9);
            auto optsl = new OptionsLayer;
            optsl->init("nh67b");
            optsl->customSetup();
            optsl->showLayer(0);
            auto songsbutton = typeinfo_cast<CCMenuItemSpriteExtra*>(
                optsl->getChildByIDRecursive("songs-button")
            );
            if (songsbutton) songsbutton->activate();
        }
        void onLevelEdit(CCObject*) {
            auto layer = LevelEditorLayer::create(
                mle_leveltools::replaceSecretCoinsByUserOnesInLevel(m_level), 0
            );
            auto mark = CCNode::create();
            mark->setID("IsMainLevelEditor");
            layer->addChild(mark);
            cocos::switchToScene(layer);
        }
        void customSetup() {
            //form_box
            auto form_box = CCLayer::create();
            if (form_box) {
                //base sets
                form_box->setID("form_box");
                form_box->setPosition(CCPoint(0.f, 116.f));
                form_box->setContentSize(CCSize(255.f, 175.f));
                form_box->ignoreAnchorPointForPosition(false);
                //add
                this->m_buttonMenu->addChild(form_box);
                //layout
                auto layout = ColumnLayout::create();
                layout->setAxisReverse(1);
                layout->setCrossAxisOverflow(0);
                form_box->setLayout(layout);
            }
            //addInputs
            auto addInput = [this, form_box](auto label = "", auto id = "", CommonFilter commonFilter = CommonFilter::Any) {
                //labelNode
                auto labelNode = CCLabelBMFont::create(label, "bigFont.fnt");
                labelNode->setID(id + std::string("/label"));
                form_box->addChild(labelNode);
                //inputNode
                auto inputNode = TextInput::create(form_box->getContentWidth(), label, "chatFont.fnt");
                inputNode->getInputNode()->setID(id);
                inputNode->getInputNode()->setMaxLabelScale(1.f);
                inputNode->setCommonFilter(commonFilter);
                inputNode->setDelegate(this);
                form_box->addChild(inputNode);
                //upd
                form_box->updateLayout();
            };
            addInput("Name", "m_levelName");
            addInput("Audio Track", "m_audioTrack", CommonFilter::Int);
            addInput("Difficulty", "m_difficulty", CommonFilter::Int);
            addInput("Stars", "m_stars", CommonFilter::Int);
            loadInputs();
            //playMusic
            if (auto GJ_playMusicBtn_001 = CCSprite::createWithSpriteFrameName("GJ_playMusicBtn_001.png")) {
                auto GJ_stopMusicBtn_001 = CCSprite::createWithSpriteFrameName("GJ_stopMusicBtn_001.png");
                auto toggleItem = CCMenuItemToggler::create(
                    GJ_playMusicBtn_001, GJ_stopMusicBtn_001,
                    this, menu_selector(LevelConfigPopup::onPlayMusic)
                );
                toggleItem->setPosition(CCPoint(138.f, 212.f));
                this->m_buttonMenu->addChild(toggleItem);
            }
            //editLevel
            if (auto GJ_viewLevelsBtn_001 = CCSprite::createWithSpriteFrameName("GJ_viewLevelsBtn_001.png")) {
                auto editLevelItem = CCMenuItemSpriteExtra::create(
                    GJ_viewLevelsBtn_001,
                    this, menu_selector(LevelConfigPopup::onLevelEdit)
                );
                editLevelItem->setPosition(CCPoint(138.f, 168.000f));
                editLevelItem->m_baseScale = 0.825;
                editLevelItem->unselected();
                this->m_buttonMenu->addChild(editLevelItem);
            }
        }
        void loadInputs() {
            if (not this->m_level) return;
            auto m_levelName = typeinfo_cast<CCTextInputNode*>(this->getChildByIDRecursive("m_levelName"));
            auto m_audioTrack = typeinfo_cast<CCTextInputNode*>(this->getChildByIDRecursive("m_audioTrack"));
            auto m_difficulty = typeinfo_cast<CCTextInputNode*>(this->getChildByIDRecursive("m_difficulty"));
            auto m_stars = typeinfo_cast<CCTextInputNode*>(this->getChildByIDRecursive("m_stars"));
            if (m_levelName) m_levelName->setString(fmt::format("{}", this->m_level->m_levelName.data()));
            if (m_audioTrack) m_audioTrack->setString(fmt::format("{}", this->m_level->m_audioTrack));
            if (m_difficulty) m_difficulty->setString(fmt::format("{}", (int)this->m_level->m_difficulty));
            if (m_stars) m_stars->setString(fmt::format("{}", this->m_level->m_stars.value()));
        }
        void applyInputs() {
            if (not this->m_level) return;
            auto m_levelName = typeinfo_cast<CCTextInputNode*>(this->getChildByIDRecursive("m_levelName"));
            auto m_audioTrack = typeinfo_cast<CCTextInputNode*>(this->getChildByIDRecursive("m_audioTrack"));
            auto m_difficulty = typeinfo_cast<CCTextInputNode*>(this->getChildByIDRecursive("m_difficulty"));
            auto m_stars = typeinfo_cast<CCTextInputNode*>(this->getChildByIDRecursive("m_stars"));
            if (m_levelName) m_level->m_levelName = m_levelName->getString();
            if (m_audioTrack) m_level->m_audioTrack = utils::numFromString<int>(m_audioTrack->getString().data()).value_or(m_level->m_audioTrack);
            if (m_difficulty) m_level->m_difficulty = (GJDifficulty)utils::numFromString<int>(m_difficulty->getString().data()).value_or((int)m_level->m_difficulty);
            if (m_stars) m_level->m_stars = utils::numFromString<int>(m_stars->getString().data()).value_or(m_level->m_stars.value());
        }
        void textChanged(CCTextInputNode* p0) {
            //log::debug("{}({}) id: {}", __FUNCTION__, p0, p0->getID());
            if (p0->getID() == "m_difficulty") {
                auto difficulty = utils::numFromString<int>(p0->getString().data()).value_or(0);
                GJDifficultySprite* difficultyPrev;
                if (difficultyPrev = typeinfo_cast<GJDifficultySprite*>(this->getChildByIDRecursive("difficultyPrev")));
                else {
                    difficultyPrev = GJDifficultySprite::create(difficulty, GJDifficultyName::Short);
                    difficultyPrev->setAnchorPoint(CCPoint(0.f, 0.f));
                    difficultyPrev->setPosition(CCPoint(260.f, 0.f));
                    difficultyPrev->setScale(1.2f);
                    difficultyPrev->setID("difficultyPrev");
                    p0->getParent()->addChild(difficultyPrev);
                }
                difficultyPrev->updateDifficultyFrame(difficulty, GJDifficultyName::Short);
            }
            if (p0->getID() == "m_audioTrack") {
                CCMenu* browseBtnMenu;
                if (browseBtnMenu = typeinfo_cast<CCMenu*>(this->getChildByIDRecursive("browseBtnMenu")));
                else {
                    //browseBtnSpr
                    auto browseBtnSpr = ButtonSprite::create("Browse", 0.5f);
                    auto browseBtn = CCMenuItemSpriteExtra::create(
                        browseBtnSpr, this, menu_selector(LevelConfigPopup::onBrowseSongs)
                    );
                    browseBtnMenu = CCMenu::createWithItem(browseBtn);
                    browseBtnMenu->setAnchorPoint(CCPoint(0.f, 0.f));
                    browseBtnMenu->setPosition(CCPoint(242.f, 6.f));
                    browseBtnMenu->setScale(0.825f);
                    browseBtnMenu->setID("browseBtnMenu");
                    p0->getParent()->addChild(browseBtnMenu);
                    handleTouchPriority(this);
                }
            }
        }
        void FLAlert_Clicked(FLAlertLayer* p0, bool p1) {
            GameManager::get()->fadeInMenuMusic();
            if (not p1) return;
            applyInputs();
            //json
            auto value = mle_leveltools::jsonFromLevel(this->m_level);
            //save json
            auto level_meta_file = levels_meta_path / fmt::format("{}.json", m_level->m_levelID.value());
            std::ofstream(level_meta_file) << value.dump(matjson::TAB_INDENTATION);
            //call custom func
            this->m_onSave();
        };
    };
    class AudioConfigPopup : public FLAlertLayer, FLAlertLayerProtocol, TextInputDelegate {
    public:
        std::function<void()> m_onSave = []() {};
        mle_audiotools::Audio m_audio = mle_audiotools::Audio(0);
        static auto create(int id = 0) {
            auto rtn = new AudioConfigPopup;
            rtn->init(
                rtn,
                "Audio Meta Config",
                "\n \n \n \n \n \n \n \n ",
                "Close", "Save", 352.000f, 0, 290.000f, 1.f
            );
            rtn->setID("AudioConfigPopup");
            rtn->m_audio = mle_audiotools::Audio(id);
            rtn->customSetup();
            return rtn;
        }
        void onPlayMusic(CCObject* toggleItemObj) {
            auto m_fileName = typeinfo_cast<CCTextInputNode*>(this->getChildByIDRecursive("m_fileName")); if (not m_fileName) return;
            auto toggleItem = typeinfo_cast<CCMenuItemToggler*>(toggleItemObj); if (not toggleItem) return;
            toggleItem->isOn() ?
                GameManager::get()->fadeInMenuMusic() :
                GameManager::get()->fadeInMusic(m_fileName->getString());
        }
        void onBrowseArtists(CCObject* asd) {
            auto pArtistsLayer = ArtistsLayer::create();
            pArtistsLayer->showLayer(0);
            pArtistsLayer->setZOrder(this->getZOrder());
        }
        void customSetup() {
            //form_box
            auto form_box = CCLayer::create();
            if (form_box) {
                //base sets
                form_box->setID("form_box");
                form_box->setPosition(CCPoint(0.f, 116.f));
                form_box->setContentSize(CCSize(255.f, 175.f));
                form_box->ignoreAnchorPointForPosition(false);
                //add
                this->m_buttonMenu->addChild(form_box);
                //layout
                auto layout = ColumnLayout::create();
                layout->setAxisReverse(1);
                layout->setCrossAxisOverflow(0);
                form_box->setLayout(layout);
            }
            //addInputs
            auto addInput = [this, form_box](auto label = "", auto id = "", CommonFilter commonFilter = CommonFilter::Any) {
                //labelNode
                auto labelNode = CCLabelBMFont::create(label, "bigFont.fnt");
                labelNode->setID(id + std::string("/label"));
                form_box->addChild(labelNode);
                //inputNode
                auto inputNode = TextInput::create(form_box->getContentWidth(), label, "chatFont.fnt");
                inputNode->getInputNode()->setID(id);
                inputNode->getInputNode()->setMaxLabelScale(1.f);
                inputNode->setCommonFilter(commonFilter);
                inputNode->setDelegate(this);
                form_box->addChild(inputNode);
                //upd
                form_box->updateLayout();
                }; 
            addInput("File Name", "m_fileName");
            addInput("Title", "m_title");
            addInput("URL", "m_url");
            addInput("Artist ID", "m_artistID", CommonFilter::Int);
            loadInputs();
            //playMusic
            if (auto GJ_playMusicBtn_001 = CCSprite::createWithSpriteFrameName("GJ_playMusicBtn_001.png")) {
                auto GJ_stopMusicBtn_001 = CCSprite::createWithSpriteFrameName("GJ_stopMusicBtn_001.png");
                auto toggleItem = CCMenuItemToggler::create(
                    GJ_playMusicBtn_001, GJ_stopMusicBtn_001,
                    this, menu_selector(AudioConfigPopup::onPlayMusic)
                );
                toggleItem->setPosition(CCPoint(138.f, 212.f));
                this->m_buttonMenu->addChild(toggleItem);
            }
        }
        void loadInputs() {
            auto m_fileName = typeinfo_cast<CCTextInputNode*>(this->getChildByIDRecursive("m_fileName"));
            auto m_title = typeinfo_cast<CCTextInputNode*>(this->getChildByIDRecursive("m_title"));
            auto m_url = typeinfo_cast<CCTextInputNode*>(this->getChildByIDRecursive("m_url"));
            auto m_artistID = typeinfo_cast<CCTextInputNode*>(this->getChildByIDRecursive("m_artistID"));
            if (m_fileName) m_fileName->setString(fmt::format("{}", this->m_audio.m_fileName.data()));
            if (m_title) m_title->setString(fmt::format("{}", this->m_audio.m_title.data()));
            if (m_url) m_url->setString(fmt::format("{}", this->m_audio.m_url.data()));
            if (m_artistID) m_artistID->setString(fmt::format("{}", this->m_audio.m_artistID));
        }
        void applyInputs() {
            auto m_fileName = typeinfo_cast<CCTextInputNode*>(this->getChildByIDRecursive("m_fileName"));
            auto m_title = typeinfo_cast<CCTextInputNode*>(this->getChildByIDRecursive("m_title"));
            auto m_url = typeinfo_cast<CCTextInputNode*>(this->getChildByIDRecursive("m_url"));
            auto m_artistID = typeinfo_cast<CCTextInputNode*>(this->getChildByIDRecursive("m_artistID"));
            if (m_fileName) this->m_audio.m_fileName = m_fileName->getString().data();
            if (m_title) this->m_audio.m_title = m_title->getString().data();
            if (m_url) this->m_audio.m_url = m_url->getString().data();
            if (m_artistID) this->m_audio.m_artistID = utils::numFromString<int>(m_artistID->getString().data()).value_or(this->m_audio.m_artistID);
        }
        void textChanged(CCTextInputNode* p0) {
            //log::debug("{}({}) id: {}", __FUNCTION__, p0, p0->getID());
            if (p0->getID() == "m_fileName") {
                CCLabelBMFont* existanceIndicator;
                if (existanceIndicator = typeinfo_cast<CCLabelBMFont*>(this->getChildByIDRecursive("existanceIndicator")));
                else {
                    existanceIndicator = CCLabelBMFont::create("", "bigFont.fnt");
                    existanceIndicator->setAnchorPoint(CCPoint(0.f, -0.200f));
                    existanceIndicator->setPosition(CCPoint(260.f, 0.f));
                    existanceIndicator->setScale(0.3f);
                    existanceIndicator->setID("existanceIndicator");
                    p0->getParent()->addChild(existanceIndicator);
                }
                auto exist = fs::exists(p0->getString().data());
                existanceIndicator->setString(exist ? "file\nexists." : "file don't\nexists!");
                existanceIndicator->setColor(exist ? ccColor3B(85, 255, 100) : ccColor3B(255, 85, 85));
            }
            if (p0->getID() == "m_artistID") {
                CCMenu* browseBtnMenu;
                if (browseBtnMenu = typeinfo_cast<CCMenu*>(this->getChildByIDRecursive("browseBtnMenu")));
                else {
                    //browseBtnSpr
                    auto browseBtnSpr = ButtonSprite::create("Browse", 0.5f);
                    auto browseBtn = CCMenuItemSpriteExtra::create(
                        browseBtnSpr, this, menu_selector(AudioConfigPopup::onBrowseArtists)
                    );
                    browseBtnMenu = CCMenu::createWithItem(browseBtn);
                    browseBtnMenu->setAnchorPoint(CCPoint(0.f, 0.f));
                    browseBtnMenu->setPosition(CCPoint(242.f, 6.f));
                    browseBtnMenu->setScale(0.825f);
                    browseBtnMenu->setID("browseBtnMenu");
                    p0->getParent()->addChild(browseBtnMenu);
                    handleTouchPriority(this);
                }
            }
        }
        void FLAlert_Clicked(FLAlertLayer* p0, bool p1) {
            GameManager::get()->fadeInMenuMusic();
            if (not p1) return;
            applyInputs();
            //json
            auto value = m_audio.resetJson();
            //save json
            auto song_meta_file = audios_meta_path / fmt::format("{}.json", m_audio.m_audioID);
            std::ofstream(song_meta_file) << value.dump(matjson::TAB_INDENTATION);
            //call custom func
            this->m_onSave();
        };
    };
    class ArtistsConfigPopup : public FLAlertLayer, FLAlertLayerProtocol, TextInputDelegate {
    public:
        std::function<void()> m_onSave = []() {};
        mle_audiotools::Artist m_artist = mle_audiotools::Artist(0);
        static auto create(int id = 0) {
            auto rtn = new ArtistsConfigPopup;
            rtn->init(
                rtn,
                "Audio Meta Config",
                "\n \n \n \n \n \n \n \n ",
                "Close", "Save", 352.000f, 0, 290.000f, 1.f
            );
            rtn->setID("ArtistsConfigPopup");
            rtn->m_artist = mle_audiotools::Artist(id);
            rtn->customSetup();
            return rtn;
        }
        void onDelete(CCObject* asd) {
            //remove json
            auto song_meta_file = artists_meta_path / fmt::format("{}.json", m_artist.m_artistID);
            fs::remove(song_meta_file, fs::last_err_code);
            //reset layer mayb ya
            this->onBtn1(this);
            this->m_onSave();
        }
        void customSetup() {
            //form_box
            auto form_box = CCLayer::create();
            if (form_box) {
                //base sets
                form_box->setID("form_box");
                form_box->setPosition(CCPoint(0.f, 116.f));
                form_box->setContentSize(CCSize(255.f, 175.f));
                form_box->ignoreAnchorPointForPosition(false);
                //add
                this->m_buttonMenu->addChild(form_box);
                //layout
                auto layout = ColumnLayout::create();
                layout->setAxisReverse(1);
                layout->setCrossAxisOverflow(0);
                form_box->setLayout(layout);
            }
            //addInputs
            auto addInput = [this, form_box](auto label = "", auto id = "", CommonFilter commonFilter = CommonFilter::Any) {
                //labelNode
                auto labelNode = CCLabelBMFont::create(label, "bigFont.fnt");
                labelNode->setID(id + std::string("/label"));
                form_box->addChild(labelNode);
                //inputNode
                auto inputNode = TextInput::create(form_box->getContentWidth(), label, "chatFont.fnt");
                inputNode->getInputNode()->setID(id);
                inputNode->getInputNode()->setMaxLabelScale(1.f);
                inputNode->setCommonFilter(commonFilter);
                inputNode->setDelegate(this);
                form_box->addChild(inputNode);
                //upd
                form_box->updateLayout();
                }; 
            addInput("YouTube URL", "m_ytURL");
            addInput("NG URL", "m_ngURL");
            addInput("Name", "m_name");
            loadInputs();
            //delete
            if (auto deleteBtnSprite = deleteButtonSprite()) {
                auto toggleItem = CCMenuItemSpriteExtra::create(
                    deleteBtnSprite,
                    this, menu_selector(ArtistsConfigPopup::onDelete)
                );
                toggleItem->setPosition(CCPoint(138.f, 212.f));
                this->m_buttonMenu->addChild(toggleItem);
            }
        }
        void loadInputs() {
            auto m_ytURL = typeinfo_cast<CCTextInputNode*>(this->getChildByIDRecursive("m_ytURL"));
            auto m_ngURL = typeinfo_cast<CCTextInputNode*>(this->getChildByIDRecursive("m_ngURL"));
            auto m_name = typeinfo_cast<CCTextInputNode*>(this->getChildByIDRecursive("m_name"));
            if (m_ytURL) m_ytURL->setString(fmt::format("{}", this->m_artist.m_ytURL.data()));
            if (m_ngURL) m_ngURL->setString(fmt::format("{}", this->m_artist.m_ngURL.data()));
            if (m_name) m_name->setString(fmt::format("{}", this->m_artist.m_name.data()));
        }
        void applyInputs() {
            auto m_ytURL = typeinfo_cast<CCTextInputNode*>(this->getChildByIDRecursive("m_ytURL"));
            auto m_ngURL = typeinfo_cast<CCTextInputNode*>(this->getChildByIDRecursive("m_ngURL"));
            auto m_name = typeinfo_cast<CCTextInputNode*>(this->getChildByIDRecursive("m_name"));
            if (m_ytURL) this->m_artist.m_ytURL = m_ytURL->getString().data();
            if (m_ngURL) this->m_artist.m_ngURL = m_ngURL->getString().data();
            if (m_name) this->m_artist.m_name = m_name->getString().data();
        }
        void FLAlert_Clicked(FLAlertLayer* p0, bool p1) {
            GameManager::get()->fadeInMenuMusic();
            if (not p1) return;
            applyInputs();
            //json
            auto value = m_artist.resetJson();
            //save json
            auto artist_meta_file = artists_meta_path / fmt::format("{}.json", m_artist.m_artistID);
            std::ofstream(artist_meta_file) << value.dump(matjson::TAB_INDENTATION);
            //call custom func
            this->m_onSave();
        };
    };
    inline void ArtistsLayer::onView(CCObject* obj) {
        auto pArtistsConfigPopup = ArtistsConfigPopup::create(obj->getTag());
        pArtistsConfigPopup->m_onSave = [this]() {
            this->hideLayer(1);
            auto newArtistsLayer = ArtistsLayer::create();
            newArtistsLayer->setZOrder(this->getZOrder());
            newArtistsLayer->showLayer(1);
            };
        pArtistsConfigPopup->show();
    };
}