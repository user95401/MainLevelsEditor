#include <_fs.hpp>
//geode
#include <Geode/Geode.hpp>
using namespace geode::prelude;

#include <Geode/utils/web.hpp>

inline auto repo = Mod::get()->getMetadata().getLinks().getSourceURL().value_or("");
inline auto raw_repo_content = string::replace(repo, "github.com", "raw.githubusercontent.com") + "/master/";
inline auto latest_release = repo + "/releases/latest/download/";

inline void download(std::string url = "", fs::path path = "", std::function<void()> onDownloadingEnd = []() {}) {
    auto layer = CCLayer::create();
    layer->setID("download"_spr);
    SceneManager::get()->keepAcrossScenes(layer);

    auto downloadingLabel = CCLabelBMFont::create(
        fmt::format("Downloading {}...", fs::path(url).filename().string()).c_str(),
        "goldFont.fnt"
    );
    layer->setID("downloadingLabel"_spr);
    downloadingLabel->setLayoutOptions(AnchorLayoutOptions::create()
        ->setAnchor(Anchor::Top)
        ->setOffset({ 0.f, -12.f }
    ));
    downloadingLabel->setScale(0.600f);
    layer->addChild(downloadingLabel);

    layer->setAnchorPoint(CCPointZero);
    layer->setLayout(AnchorLayout::create());

    auto req = web::WebRequest();
    auto listener = new EventListener<web::WebTask>;
    listener->bind(
        [path, onDownloadingEnd, layer, downloadingLabel](web::WebTask::Event* e) {
            if (web::WebProgress* prog = e->getProgress()) {
                double a100 = 100 - prog->downloadProgress().value_or(0.0);
                if (downloadingLabel and (a100 > 0.0)) downloadingLabel->setOpacity((a100 / 100) * 255);
            }
            if (web::WebResponse* res = e->getValue()) {
                std::string data = res->string().unwrapOr("no res");
                if (res->code() < 399) {
                    res->into(path);
                    if (layer) SceneManager::get()->forget(layer);
                    if (layer) layer->removeFromParent();
                    onDownloadingEnd();
                }
                else {
                    auto asd = geode::createQuickPopup(
                        "Request exception",
                        data,
                        "Nah", nullptr, 420.f, nullptr, false
                    );
                    asd->show();
                    if (layer) SceneManager::get()->forget(layer);
                    if (layer) layer->removeFromParent();
                };
            }
        }
    );
    listener->setFilter(req.send("GET", url));

}

inline auto UPDATES_CHECK_DISABLED = Mod::get()->getSaveDir() / "UPDATES_CHECK_DISABLED.txt";
inline auto UPDATES_SKIPPED = Mod::get()->getSaveDir() / "UPDATES_SKIPPED.txt";

inline auto doWithConfirmPop(auto lb = []() {}) {
    return geode::createQuickPopup(
        "Action Confirm",
        "Are you sure?",
        "Abort", "Yes",
        [lb](auto, bool btn2) {
            if (btn2) lb();
        }
    );
}

#include <Geode/modify/MenuLayer.hpp>
class $modify(MenuLayerExt, MenuLayer) {
    struct Fields {
        EventListener<web::WebTask> m_getJsonListener;
    };
    $override bool init() {
        
        //get mod json
        m_fields->m_getJsonListener.bind(
            [this](web::WebTask::Event* e) {
                if (web::WebResponse* res = e->getValue()) {

                    auto str = res->string().unwrapOr("");

                    auto parse = matjson::parse(str);

                    if (not parse.ok()) 
                        return log::error("parse err: {}", parse.unwrapErr());

                    auto actualMetaDataResult = ModMetadata::create(parse.unwrap());

                    if (not actualMetaDataResult.ok())
                        return log::error("actualMetaDataResult: {}", actualMetaDataResult.unwrapErr());

                    auto actualMetaData = actualMetaDataResult.unwrap();

                    if (actualMetaData.getVersion() == Mod::get()->getVersion()) return;

                    auto updatesSkipped = fs::read(UPDATES_SKIPPED);
                    if (string::contains(
                        updatesSkipped, "\"" + actualMetaData.getVersion().toVString() + "\""
                    )) return;

                    auto pop = geode::createQuickPopup(
                        Mod::get()->getID().c_str(),
                        fmt::format(
                            "\n" "Founded <cy>new</c> actual <cy>version</c>!"
                            "\n" "<cb>{}</c> <ca>-></c> <cj>{}</c>"
                            "\n" "<co>Download latest release of mod?</c>"
                            "\n "
                            , Mod::get()->getVersion().toVString(), actualMetaData.getVersion().toVString()
                        ),
                        "", "", [](CCNode*pop, auto) {
                            SceneManager::get()->forget(pop);
                            pop->removeFromParent();
                        }, false
                    );
                    pop->show();
                    SceneManager::get()->keepAcrossScenes(pop);

                    pop->m_buttonMenu->removeAllChildrenWithCleanup(0);
                    pop->m_buttonMenu->setScale(0.6f);
                    pop->m_buttonMenu->setAnchorPoint(CCPoint(0.5f, 0.3f));
                    pop->m_buttonMenu->setContentSize(CCSize(512.f, 65.f));

                    pop->m_buttonMenu->addChild(CCMenuItemExt::createSpriteExtra(
                        ButtonSprite::create("Download And Restart", "goldFont.fnt", "GJ_button_01.png", 1.f),
                        [pop](auto) {
                            pop->m_alertProtocol->FLAlert_Clicked(pop, 0);
                            download(
                                latest_release + Mod::get()->getID() + ".geode",
                                dirs::getModsDir() / (Mod::get()->getID() + ".geode"),
                                []() {
                                    game::restart();
                                }
                            );
                        }
                    ), 0, 1);
                    pop->m_buttonMenu->getChildByTag(1)->setLayoutOptions(AxisLayoutOptions::create()->setBreakLine(1));

                    pop->m_buttonMenu->addChild(CCMenuItemExt::createSpriteExtra(
                        ButtonSprite::create("Just Download", "goldFont.fnt", "GJ_button_04.png", 0.6f),
                        [pop](auto) {
                            pop->m_alertProtocol->FLAlert_Clicked(pop, 0);
                            download(
                                latest_release + Mod::get()->getID() + ".geode",
                                dirs::getModsDir() / (Mod::get()->getID() + ".geode"),
                                []() {
#ifdef __APPLE__
                                    Notification::create(
                                        "Latest release downloading is finished!",
                                        CCSprite::createWithSpriteFrameName("geode.loader/install.png")
                                    )->show();
#else
                                    AchievementNotifier::sharedState()->notifyAchievement(
                                        Mod::get()->getID().c_str(),
                                        "Latest release downloading is finished!",
                                        "geode.loader/install.png", 1
                                    );
#endif // __APPLE__
                                }
                            );
                        }
                    ), 0, 2);

                    pop->m_buttonMenu->addChild(CCMenuItemExt::createSpriteExtra(
                        ButtonSprite::create("Later", "goldFont.fnt", "GJ_button_04.png", 0.6f),
                        [pop](auto) {
                            pop->m_alertProtocol->FLAlert_Clicked(pop, 0);
                        }
                    ), 0, 3);

                    pop->m_buttonMenu->addChild(CCMenuItemExt::createSpriteExtra(
                        ButtonSprite::create("Skip", "goldFont.fnt", "GJ_button_05.png", 0.6f),
                        [pop, actualMetaData](auto) { doWithConfirmPop([pop, actualMetaData]() {

                            std::ofstream(UPDATES_SKIPPED, std::ios_base::app)
                                << "\"" + actualMetaData.getVersion().toVString() + "\""
                                << std::endl;
                            pop->m_alertProtocol->FLAlert_Clicked(pop, 0);

                            });}
                    ), 0, 4);

                    pop->m_buttonMenu->addChild(CCMenuItemExt::createSpriteExtra(
                        ButtonSprite::create("Disable Checks", "goldFont.fnt", "GJ_button_06.png", 0.6f),
                        [pop](auto) { doWithConfirmPop([pop]() {

                            std::ofstream(UPDATES_CHECK_DISABLED)
                                << "delete it to bring back checks";
                            pop->m_alertProtocol->FLAlert_Clicked(pop, 0);

                            });}
                    ), 0, 5);

                    pop->m_buttonMenu->setLayout(AxisLayout::create()->setGrowCrossAxis(1));
                }
            }
        );
        if (fs::exists(UPDATES_CHECK_DISABLED)) void();
        else m_fields->m_getJsonListener.setFilter(
            web::WebRequest().get(raw_repo_content + "/mod.json")
        );

        return MenuLayer::init();
    }
};