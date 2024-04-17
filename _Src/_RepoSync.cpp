//geode
#include <Geode/Geode.hpp>
using namespace geode::prelude;

/*
ADD THIS IN FRONT (or ANYWHERE) OF YOUR CMakeLists.txt:

add_compile_definitions(GEODE_EXPOSE_SECRET_INTERNALS_IN_HEADERS_DO_NOT_DEFINE_PLEASE 1) #ok

*/
#include <regex>
std::string api() {
    auto meta = Mod::get()->getMetadata();
    auto repo = meta.getRepository()
        .value_or(fmt::format(
            "https://github.com/{}/{}",
            meta.getDeveloper(),
            meta.getID().replace(0, meta.getDeveloper().size() + 1, "")
        ));
    std::string repoapi = std::regex_replace(
        repo,
        std::regex("https://github.com/"),
        "https://ungh.cc/repos/"
    );
    return repoapi;
};

#include <Geode/utils/web.hpp>
void updateModByRepo() {
    //https://ungh.cc/repos/user95401/curly-eureka
    web::AsyncWebRequest()
        .fetch(api())
        .json()
        .then(
            [](matjson::Value json) {
                if (!json.contains("repo")) return;
                auto meta = Mod::get()->getMetadata();
                meta.setDescription(json["repo"]["description"].as_string());
                Mod::get()->setMetadata(meta);
            }
    );
    //https://ungh.cc/repos/user95401/curly-eureka/readme
    web::AsyncWebRequest()
        .fetch(api()+"/readme")
        .json()
        .then(
            [](matjson::Value json) {
                if (!json.contains("markdown")) return;
                auto meta = Mod::get()->getMetadata();
                meta.setDetails(json["markdown"].as_string());
                Mod::get()->setMetadata(meta);
            }
    );
    //https://ungh.cc/repos/user95401/curly-eureka/releases
    web::AsyncWebRequest()
        .fetch(api()+"/releases")
        .json()
        .then(
            [](matjson::Value json) {
                if (!json.contains("releases")) return;
                if (!json["releases"].is_array()) return;
                if (json["releases"].as_array().empty()) return;
                auto meta = Mod::get()->getMetadata();
                std::stringstream releases;
                for (auto release : json["releases"].as_array()) {
                    releases << "# " << release["name"].as_string() << std::endl;
                    releases << "- Tag: " << release["tag"].as_string() << std::endl;
                    releases << "- Author: " << release["author"].as_string() << std::endl;
                    releases << "- Published At: " << release["publishedAt"].as_string() << std::endl;
                    releases << std::endl << release["markdown"].as_string() << std::endl;
                }
                meta.setChangelog(releases.str());
                Mod::get()->setMetadata(meta);
            }
    );
}

$on_mod(Loaded) {
	updateModByRepo();
};
