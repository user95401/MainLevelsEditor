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
namespace my_fs {
    using namespace std::filesystem;
    inline auto last_err = std::string();
    inline std::error_code last_err_code;
    template <typename T> inline auto rtnWithErrLog(T rtn, std::string log) { log::error("{}", log); return rtn; }
    inline auto exists(path path, std::string& err = last_err) {
        return cocos::fileExistsInSearchPaths(path.string().c_str());
    }
    inline auto read(path path, std::string& err = last_err) {
        err = fmt::format("{}(path \"{}\") err: path dnt exists", __func__, path);
        if (not my_fs::exists(path)) return rtnWithErrLog(err, err); err = "";

        unsigned long file_size = 0;
        auto buffer = CCFileUtils::sharedFileUtils()->getFileData(path.string().c_str(), "rb", &file_size);

        std::string data = "read failed...";
        if (buffer && file_size != 0) data = std::string(reinterpret_cast<char*>(buffer), file_size);
        else return rtnWithErrLog(data, err);

        return data;
    }
    inline auto rename(path old_path, path new_path, std::string& err = last_err) {
        err = fmt::format("{}(old_path \"{}\", old_path \"{}\") err: old path dnt exists", __func__, old_path, new_path);
        if (not my_fs::exists(old_path)) return rtnWithErrLog(false, err);
        err = fmt::format("{}(old_path \"{}\", old_path \"{}\") err: new path exists", __func__, old_path, new_path);
        if (my_fs::exists(new_path)) return rtnWithErrLog(false, err);
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
        auto file_content = my_fs::read(level_meta_file);
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
};

namespace mle_audiotools {
    class Audio {
    public:
        std::string m_fileName;
        std::string m_title;
        std::string m_url;
        int m_artistID;
        int m_audioID;
        matjson::Value m_json;
        inline static bool CallDefaults = false;
        inline static void updateFromFile(Audio* pAudio) {
            //update by existing
            auto song_meta_file = audios_meta_path / fmt::format("{}.json", pAudio->m_audioID);
            if (!my_fs::exists(song_meta_file)) return;
            auto file_content = my_fs::read(song_meta_file);
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
            this->m_json = matjson::Value();
            this->m_json.try_set("m_fileName", this->m_fileName);
            this->m_json.try_set("m_title", this->m_title);
            this->m_json.try_set("m_url", this->m_url);
            this->m_json.try_set("m_artistID", this->m_artistID);
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
            if (!my_fs::exists(song_meta_file)) return;
            auto file_content = my_fs::read(song_meta_file);
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
            this->m_json = matjson::Value();
            this->m_json.try_set("m_ytURL", this->m_ytURL);
            this->m_json.try_set("m_ngURL", this->m_ngURL);
            this->m_json.try_set("m_name", this->m_name);
            //save json
            if (not noDefaults and this->m_name == " ") return;
            auto song_meta_file = artists_meta_path / fmt::format("{}.json", id);
            std::ofstream(song_meta_file) << this->m_json.dump(matjson::TAB_INDENTATION);
        };
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
    class LevelConfigPopup : public FLAlertLayer, FLAlertLayerProtocol, TextInputDelegate {
    public:
        GJGameLevel* m_level;
        std::function<void()> m_onSave = []() {};
        inline static auto create(GJGameLevel* for_level = LevelTools::getLevel(1, 0)) {
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
        inline void customSetup() {
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
                auto inputNode = TextInput::create(form_box->getContentWidth(), label, "gjFont41.fnt");
                inputNode->getInputNode()->setID(id);
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
        }
        inline void loadInputs() {
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
        inline void applyInputs() {
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
        inline void textChanged(CCTextInputNode* p0) {
            log::debug("{}({}) id: {}", __FUNCTION__, p0, p0->getID());
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
        }
        inline void FLAlert_Clicked(FLAlertLayer* p0, bool p1) {
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
}