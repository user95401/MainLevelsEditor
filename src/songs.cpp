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
		return not Audio::CallDefaults ? Audio(p0).m_artistID : LevelTools::artistForAudio(p0);
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
class $modify(SongsLayer) {
	$override void customSetup() {
		SongsLayer::customSetup();
		//auto contentLayer = m_listLayer->m_listView;//->m_tableView->m_contentLayer;
		//contentLayer->setRotation(12.f);
		//contentLayer->removeAllChildrenWithCleanup(0);
	}
};