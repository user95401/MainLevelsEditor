#include "_main.hpp"

using namespace mle_audiotools;

#include <Geode/modify/LevelTools.hpp>
class $modify(LevelTools) {
	/// AUDIO
	$override static gd::string getAudioFileName(int p0) {
		return not Audio::CallDefaults ? gd::string(Audio(p0).m_fileName.data()) : LevelTools::getAudioFileName(p0);
	};
	$override static gd::string getAudioTitle(int p0) {
		return not Audio::CallDefaults ? gd::string(Audio(p0).m_title.data()) : LevelTools::getAudioTitle(p0);
	};
	$override static gd::string urlForAudio(int p0) {
		return not Audio::CallDefaults ? gd::string(Audio(p0).m_url.data()) : LevelTools::urlForAudio(p0);
	};
	$override static int artistForAudio(int p0) {
		if (SETTING(bool, "LevelTools::artistForAudio moment")) {
			return not Audio::CallDefaults ? Audio(p0).m_artistID : 1;
		}
		else return not Audio::CallDefaults ? Audio(p0).m_artistID : LevelTools::artistForAudio(p0);
	};
	/// ARTIST
	$override static gd::string ytURLForArtist(int p0) {
		return not Artist::CallDefaults ? gd::string(Artist(p0).m_ytURL.data()) : LevelTools::ytURLForArtist(p0);
	};
	$override static gd::string ngURLForArtist(int p0) {
		return not Artist::CallDefaults ? gd::string(Artist(p0).m_ngURL.data()) : LevelTools::ngURLForArtist(p0);
	};
	$override static gd::string nameForArtist(int p0) {
		return not Artist::CallDefaults ? gd::string(Artist(p0).m_name.data()) : LevelTools::nameForArtist(p0);
	};
};

#include <Geode/modify/SongsLayer.hpp>
class $modify(SongsLayerExt, SongsLayer) {
	void onView(CCObject* obj) {
		auto audio = Audio(obj->getTag());
		audio.openInfoLayer();
	}
	void onViewArtists(CCObject* obj) {
		auto pArtistsLayer = mle_ui::ArtistsLayer::create();
		pArtistsLayer->showLayer(0);
		pArtistsLayer->setZOrder(this->getZOrder());
	}
	auto addSongCell(int id, bool &altBg, CCContentLayer* contentLayer) {
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
		auto audio = Audio(id);
		auto artist = Artist(audio.m_artistID);
		auto btnAspectWidth = width - 100.0f;
		auto nameStr = fmt::format("{}", audio.m_title);
		if (SETTING(bool, "ui")) nameStr = fmt::format("{} [id: {}]", audio.m_title, audio.m_audioID);
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
		auto artistStr = fmt::format("By {}", artist.m_name);
		if (SETTING(bool, "ui")) artistStr = fmt::format("By {} [id: {}]", artist.m_name, audio.m_artistID);
		if (auto artistLabel = CCLabelBMFont::create(artistStr.c_str(), "goldFont.fnt")) {
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
				CCMenuItemSpriteExtra::create(viewBtnSprite, this, menu_selector(SongsLayerExt::onView))
			);
			menu->getChildByType<CCMenuItemSpriteExtra>(0)->setTag(id);
			menu->getChildByType<CCMenuItemSpriteExtra>(0)->setTag(id);
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
	$override void customSetup() {
		SongsLayer::customSetup();
		auto customListView = cocos::findFirstChildRecursive<CustomListView>(this->m_listLayer, [](auto e) { return true; });
		if (not customListView) return;
		customListView->setVisible(0);
		customListView->setPosition(CCPoint(customListView->getContentSize().width, 0.f));
		//ScrollLayer::create
		auto scrollLayer = ScrollLayer::create(customListView->getContentSize());
		scrollLayer->setID("scrollLayer"_spr);
		this->m_listLayer->addChild(scrollLayer);
		//fill contentLayer
		auto& contentLayer = scrollLayer->m_contentLayer; 
		contentLayer->setContentHeight(0.f);
		contentLayer->setLayout(
			ColumnLayout::create()
			->setGap(0)
			->setAxisReverse(true)
			->setAxisAlignment(AxisAlignment::End)
		);
		auto altBg = true;
		//amount
		int id = SETTING(int64_t, "songs_start_id");
		int max_id = SETTING(int64_t, "songs_max_id");
		for (id; id <= max_id; id++) addSongCell(id, altBg, contentLayer);
		//songs_special_ids
		for (auto strID : string::explode(",", SETTING(std::string, "songs_special_ids"))) {
			auto id_num = utils::numFromString<int>(strID);
			if (id_num.isOk()) addSongCell(id_num.unwrapOrDefault(), altBg, contentLayer);
		}
		//practice
        if (SETTING(bool, "practice_music")) addSongCell(-1, altBg, contentLayer);
		//update after adding
		contentLayer->setContentHeight(
			contentLayer->getContentHeight() > contentLayer->getParent()->getContentHeight() ?
			contentLayer->getContentHeight() :
			contentLayer->getParent()->getContentHeight()
		);
		contentLayer->updateLayout();
		scrollLayer->scrollToTop();
		//ui
		if (SETTING(bool, "ui")) {
			CCMenuItemSpriteExtra* viewArtists; {
				viewArtists = CCMenuItemSpriteExtra::create(
					ButtonSprite::create("View Artists"),
					this, menu_selector(SongsLayerExt::onViewArtists)
				);
				viewArtists->setID("viewArtists"_spr);
				viewArtists->setPosition({
					(this->m_buttonMenu->getContentSize().width / 2) - this->m_buttonMenu->getPositionX(),
					(this->m_buttonMenu->getContentSize().height / -1) + 48.f
					});
				viewArtists->m_baseScale = 0.8f;
				viewArtists->setScale(viewArtists->m_baseScale);
			};
			this->m_buttonMenu->addChild(viewArtists);
		}
	}
};

#include <Geode/modify/CustomSongWidget.hpp>
class $modify(CustomSongWidgetExt, CustomSongWidget) {
	void onMore(cocos2d::CCObject * sender) {
		CustomSongWidget::onMore(sender);
		if (auto openedJustNowPop = cocos::findFirstChildRecursive<SongInfoLayer>(CCDirector::get()->m_pRunningScene, [](auto) {return true; })) {
			openedJustNowPop->setTag(this->m_customSongID);
			if (this->m_isRobtopSong) openedJustNowPop->addChild(createDataNode("m_isRobtopSong"));
			if (this->m_isMusicLibrary) openedJustNowPop->addChild(createDataNode("m_isMusicLibrary"));
			if (m_songInfoObject->m_isUnknownSong) openedJustNowPop->addChild(createDataNode("m_isUnknownSong"));
			openedJustNowPop->addChild(createDataNode("m_songID", "", m_songInfoObject->m_songID));
			openedJustNowPop->addChild(createDataNode("m_songName", m_songInfoObject->m_songName.data()));
			openedJustNowPop->addChild(createDataNode("m_artistName", m_songInfoObject->m_artistName.data()));
			openedJustNowPop->addChild(createDataNode("m_youtubeVideo", m_songInfoObject->m_youtubeVideo.data()));
			openedJustNowPop->addChild(createDataNode("m_youtubeChannel", m_songInfoObject->m_youtubeChannel.data()));
			openedJustNowPop->addChild(createDataNode("m_songUrl", m_songInfoObject->m_songUrl.data()));
			openedJustNowPop->addChild(createDataNode("m_artistID", "", m_songInfoObject->m_artistID));
		}
	}
};