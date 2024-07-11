#pragma once
#include <Geode/Geode.hpp>
using namespace geode::prelude;

//lol
#define SETTING(type, key_name) Mod::get()->getSettingValue<type>(key_name)

inline auto levels_path = Mod::get()->getConfigDir() / "levels";
inline auto levels_meta_path = levels_path / "_meta";
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